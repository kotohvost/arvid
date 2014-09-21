
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <linux/fs.h> 

#include "avdfs_mount.h"

int main (int argc, char **argv)
{
  char *data = NULL;
  int rc;

  if (argc == 1)
    rc = mount (MOUNT_INODE, MOUNT_INODE, "avdfs", MS_NOSUID|MS_NODEV|MS_NOEXEC|MS_MGC_VAL, data);
  else
    rc = umount (MOUNT_INODE);

  if (rc)
    fprintf (stderr, "Can't (u)mount filesystem, rc: %d\n", rc);

  return rc;
}
