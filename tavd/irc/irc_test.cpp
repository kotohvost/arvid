#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>

#include "vcr.h"
#include "rle.h"
#include "irc.h"

struct VCR_ALL  vcr;    // один vcr на все
struct RLE      rle;    // один на всех
struct IRC      irc;    // один за всех

int load_vcr (char *fname, struct VCR_ALL *vcr)
{
  int fdin, rc = 0;
  unsigned int size;

#ifdef UNIX
  if ((fdin = open (fname, O_RDONLY)) < 0)
    return -1;
#else
  if ((fdin = open (fname, O_RDONLY | O_BINARY)) < 0)
    return -1;
#endif

  size = read (fdin, vcr, sizeof (struct VCR_ALL));

  if (size < sizeof (struct VCR_ALL) )
   {
    fprintf (stdout, "load_vcr: size = %d != %d\n", size, (unsigned int)sizeof (struct VCR_ALL));
    rc = -1;
   }

  close (fdin);
  return rc;
}

void print_info_vcr (struct VCR_ALL *vcr)
{
  int i;

  // print firm_name
  for (i = 0; i < FIRM_LEN && vcr->info.firm_name[i]; i++)
    fprintf (stdout, "%c", vcr->info.firm_name[i]);

  fprintf (stdout, "-");

  // print vcr_name
  for (i = 0; i < VCR_LEN && vcr->info.vcr_name[i]; i++)
    fprintf (stdout, "%c", vcr->info.vcr_name[i]);

  fprintf(stdout, "\n");
}

void vcr_initwr (struct VCR_ALL *vcr, int control, struct RLE *rle, struct IRC *irc)
{
  rle_init (rle, control, &vcr->tbl.ik);
  irc_initwr (irc,
              rle_getbuf(rle),
              vcr->tbl.ik.irc_header.period,
              vcr->tbl.ik.irc_type);
}

int main (int argc, char *argv[])
{
  struct VCR *svcr;

  if (argc == 1)
   {
    fprintf (stderr, "Usage: %s <file.vcr> #keynum\n", argv[0]);
    return 1;
   }

  svcr = open_vcr (argv[1]);

  if (svcr == NULL)
   {
    fprintf (stderr, "Error open_vcr('%s')!\n", argv[1]);
    return 1;
   }

  close_vcr (svcr);
  fprintf (stdout, "svcr = %u\n", (unsigned int)svcr);

  if (load_vcr (argv[1], &vcr))
   {
    fprintf (stderr, "Error loading file '%s'!\n", argv[1]);
    fprintf (stderr, "'%s'\n", strerror (errno));
    return 1;
   }

  print_info_vcr (&vcr);

  if (argc > 2)
   {
    vcr_initwr(&vcr, atoi(argv[2]), &rle, &irc);

#if 0
    unsigned int pause;
    pause = irc_getperiod (&irc) * rle_getmaxpause (&rle) / 3000;

    if (pause < 30)
      pause = 40;

// irc_init(&irc);
#endif
   }

  return 0;
}
