#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>

#include "vcr.h"
#include "rle.h"
#include "irc.h"

// Open VCR file. Return &struct.
struct VCR *open_vcr (char *fname)
{
  int         fdin;
  struct stat fdinstat;
  struct VCR  *vcr;

  // init
  vcr = NULL;

  // check exist
  if (access (fname, 0))
    return NULL;

  // get file size & check size
  if (stat (fname, &fdinstat) != 0)
    return NULL;

  if ((unsigned int)fdinstat.st_size < sizeof (struct VCR_ALL))
    return NULL;

  // open file
#ifdef UNIX
  if ((fdin = open (fname, O_RDONLY)) < 0)
    return NULL;
#else
  if ((fdin = open (fname, O_RDONLY | O_BINARY)) < 0)
    return NULL;
#endif

  // alloc mem for VCR
  vcr = (struct VCR*) malloc (sizeof (struct VCR));

  if (vcr == NULL)
    return NULL;

  // alloc mem for VCR_ALL
  vcr->vcr_size = fdinstat.st_size;
  vcr->vcr = (struct VCR_ALL*) malloc (vcr->vcr_size);

  if (vcr->vcr == NULL)
   {
    free(vcr);
    return NULL;
   }

  // read file
  if (read (fdin, vcr->vcr, vcr->vcr_size) < vcr->vcr_size)
   {
    free (vcr->vcr);
    free (vcr);
    return NULL;
   }

  // initialize RLE
  vcr->rle = (struct RLE*) malloc (sizeof (struct RLE));

  if (vcr->rle != NULL)
    rle_init (vcr->rle, -1, &vcr->vcr->tbl.ik);

  // initialize other pointer
  vcr->irc = NULL;

  close(fdin);
  return vcr;
}

// Close VCR file
void close_vcr (struct VCR *vcr)
{
  if (vcr == NULL)
    return;

  if (vcr->irc)
    free (vcr->irc);

  if (vcr->rle)
    free (vcr->rle);

  if (vcr->vcr)
    free (vcr->vcr);

  free (vcr);

  // hit!
  vcr = NULL;
}
