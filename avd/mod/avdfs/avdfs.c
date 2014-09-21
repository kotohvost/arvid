
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/locks.h>

#include <asm/uaccess.h>

/* some random number */
#define AVDFS_MAGIC	0xBAADF00D

static struct super_operations         avdfs_ops;
static struct address_space_operations avdfs_aops;
static struct file_operations          avdfs_dir_operations;
static struct file_operations          avdfs_file_operations;
static struct inode_operations         avdfs_dir_inode_operations;

static int avdfs_statfs (struct super_block *sb, struct statfs *buf)
{
  buf->f_type  = AVDFS_MAGIC;
  buf->f_bsize = PAGE_CACHE_SIZE;
  buf->f_namelen = 255;
  
  return 0;
}

/* Lookup the data. This is trivial - if the dentry didn't already
 * exist, we know it is negative. */
 
static struct dentry * avdfs_lookup (struct inode *dir, struct dentry *dentry)
{
  d_add (dentry, NULL);
  
  return NULL;
}

/* Read a page. Again trivial. If it didn't already exist
 * in the page cache, it is zero-filled. */
 
static int avdfs_readpage (struct file *file, struct page * page)
{
  if (!Page_Uptodate(page))
   {
    memset (kmap(page), 0, PAGE_CACHE_SIZE);
    kunmap (page);
    flush_dcache_page (page);
    SetPageUptodate (page);
   }
   
  UnlockPage (page);
  return 0;
}

static int avdfs_prepare_write (struct file *file, struct page *page, unsigned offset, unsigned to)
{
  void *addr = kmap (page);
  
  if (!Page_Uptodate (page))
   {
    memset (addr, 0, PAGE_CACHE_SIZE);
    flush_dcache_page (page);
    SetPageUptodate (page);
   }
   
  SetPageDirty (page);
  return 0;
}

static int avdfs_commit_write (struct file *file, struct page *page, unsigned offset, unsigned to)
{
  struct inode *inode = page->mapping->host;
  loff_t pos = ((loff_t)page->index << PAGE_CACHE_SHIFT) + to;

  kunmap (page);
  
  if (pos > inode->i_size)
    inode->i_size = pos;
    
  return 0;
}

struct inode *avdfs_get_inode(struct super_block *sb, int mode, int dev)
{
  struct inode * inode = new_inode(sb);

  if (inode)
   {
    inode->i_mode = mode;
    inode->i_uid = current->fsuid;
    inode->i_gid = current->fsgid;
    inode->i_blksize = PAGE_CACHE_SIZE;
    inode->i_blocks = 0;
    inode->i_rdev = NODEV;
    inode->i_mapping->a_ops = &avdfs_aops;
    inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
    
    switch (mode & S_IFMT)
     {
           default: init_special_inode (inode, mode, dev); break;
      case S_IFREG: inode->i_fop = &avdfs_file_operations; break;
      case S_IFDIR: inode->i_op = &avdfs_dir_inode_operations;
		    inode->i_fop = &avdfs_dir_operations;  break;
      case S_IFLNK: inode->i_op = &page_symlink_inode_operations; break;
     }
   }
   
  return inode;
}

/* File creation. Allocate an inode, and we're done.. */

static int avdfs_mknod (struct inode *dir, struct dentry *dentry, int mode, int dev)
{
  struct inode * inode = avdfs_get_inode (dir->i_sb, mode, dev);
  int error = -ENOSPC;

  if (inode)
   {
    d_instantiate (dentry, inode);
    dget (dentry);  /* Extra count - pin the dentry in core */
    error = 0;
   }
   
  return error;
}

static int avdfs_mkdir (struct inode * dir, struct dentry * dentry, int mode)
{
  return avdfs_mknod (dir, dentry, mode | S_IFDIR, 0);
}

static int avdfs_create (struct inode *dir, struct dentry *dentry, int mode)
{
  return avdfs_mknod (dir, dentry, mode | S_IFREG, 0);
}

/* Link a file.. */

static int avdfs_link (struct dentry *old_dentry, struct inode * dir, struct dentry * dentry)
{
  struct inode *inode = old_dentry->d_inode;

  if (S_ISDIR(inode->i_mode))
    return -EPERM;

  inode->i_nlink++;
  atomic_inc (&inode->i_count);	/* New dentry reference */
  dget (dentry);		/* Extra pinning count for the created dentry */
  d_instantiate (dentry, inode);
  
  return 0;
}

static inline int avdfs_positive (struct dentry *dentry)
{
  return dentry->d_inode && !d_unhashed(dentry);
}

/* Check that a directory is empty (this works
 * for regular files too, they'll just always be
 * considered empty..).
 *
 * Note that an empty directory can still have
 * children, they just all have to be negative.. */
 
static int avdfs_empty (struct dentry *dentry)
{
  struct list_head *list;

  spin_lock (&dcache_lock);
  list = dentry->d_subdirs.next;

  while (list != &dentry->d_subdirs)
   {
    struct dentry *de = list_entry(list, struct dentry, d_child);

    if (avdfs_positive(de))
     {
      spin_unlock(&dcache_lock);
      return 0;
     }
     
    list = list->next;
   }
   
  spin_unlock(&dcache_lock);
  return 1;
}

/* This works for both directories and regular files.
 * (non-directories will always have empty subdirs) */
 
static int avdfs_unlink (struct inode * dir, struct dentry *dentry)
{
  int retval = -ENOTEMPTY;

  if (avdfs_empty(dentry))
   {
    struct inode *inode = dentry->d_inode;

    inode->i_nlink--;
    dput (dentry); /* Undo the count from "create" - this does all the work */
    retval = 0;
   }
   
  return retval;
}

#define avdfs_rmdir avdfs_unlink

/* The VFS layer already does all the dentry stuff for rename,
 * we just have to decrement the usage count for the target if
 * it exists so that the VFS layer correctly free's it when it
 * gets overwritten. */
 
static int avdfs_rename (struct inode * old_dir, struct dentry *old_dentry, struct inode * new_dir,struct dentry *new_dentry)
{
  int error = -ENOTEMPTY;

  if (avdfs_empty(new_dentry))
   {
    struct inode *inode = new_dentry->d_inode;
    
    if (inode)
     {
      inode->i_nlink--;
      dput (new_dentry);
     }
     
    error = 0;
   }
   
  return error;
}

static int avdfs_symlink (struct inode * dir, struct dentry *dentry, const char * symname)
{
  int error = avdfs_mknod (dir, dentry, S_IFLNK | S_IRWXUGO, 0);
  
  if (!error)
   {
    int l = strlen (symname) + 1;
    struct inode *inode = dentry->d_inode;
    error = block_symlink (inode, symname, l);
   }
   
  return error;
}

static int avdfs_sync_file (struct file * file, struct dentry *dentry, int datasync)
{
  return 0;
}

/*	writepage:	avdfs_writepage, */
static struct address_space_operations avdfs_aops = {
	readpage:	avdfs_readpage,
	prepare_write:	avdfs_prepare_write,
	commit_write:	avdfs_commit_write
};

static struct file_operations avdfs_file_operations = {
	read:		generic_file_read,
	write:		generic_file_write,
	mmap:		generic_file_mmap,
	fsync:		avdfs_sync_file,
};

static struct file_operations avdfs_dir_operations = {
	read:		generic_read_dir,
	readdir:	dcache_readdir,
	fsync:		avdfs_sync_file,
};

static struct inode_operations avdfs_dir_inode_operations = {
	create:		avdfs_create,
	lookup:		avdfs_lookup,
	link:		avdfs_link,
	unlink:		avdfs_unlink,
	symlink:	avdfs_symlink,
	mkdir:		avdfs_mkdir,
	rmdir:		avdfs_rmdir,
	mknod:		avdfs_mknod,
	rename:		avdfs_rename,
};

static struct super_operations avdfs_ops = {
	statfs:		avdfs_statfs,
	put_inode:	force_delete,
};

static struct super_block *avdfs_read_super (struct super_block * sb, void * data, int silent)
{
  struct inode * inode;
  struct dentry * root;

  sb->s_blocksize = PAGE_CACHE_SIZE;
  sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
  sb->s_magic = AVDFS_MAGIC;
  sb->s_op = &avdfs_ops;
  inode = avdfs_get_inode (sb, S_IFDIR | 0755, 0);
	
  if (!inode)
    return NULL;

  root = d_alloc_root(inode);
  
  if (!root)
   {
    iput (inode);
    return NULL;
   }
 
  sb->s_root = root;
  return sb;
}

static DECLARE_FSTYPE(avdfs_fs_type, "avdfs", avdfs_read_super, FS_LITTER);

static int __init init_avdfs_fs (void)
{
  return register_filesystem (&avdfs_fs_type);
}

static void __exit exit_avdfs_fs (void)
{
  unregister_filesystem (&avdfs_fs_type);
}

module_init (init_avdfs_fs)
module_exit (exit_avdfs_fs)
