
#include <stdio.h>
#include <stdarg.h>

#include "defines.h"
#include "dprint.h"

bool DFlag = true; /* true - debug output avaible */
int  VerbFlag = 5, AnimPhase = 0; /* 0 - minimal verbose, 5 - maximal verbose */
char AnimTable [] = "|/-\\"; /* progress indicators template */

void Progress (void)
{
  printf ("%c\r", AnimTable [AnimPhase & 3]);
  fflush (stdout);
  AnimPhase++;
}

void print_data (int Verb, byte *str, int max_len)
{
  int i = 0;
  
  if (DFlag && Verb > VerbFlag)
    return;
  
  while (str[i] && i < max_len)
   {
    if (str[i] > 0x20)
      printf ("%c", str[i]);
      
    i++;
   }
   
  printf ("\n"); 
}

void dprintf (int Verb, const char *str, ...)
{
  if (DFlag && Verb <= VerbFlag)
   {
    char buf [7777];
    va_list arg;

    va_start (arg, str);
    vsprintf (buf, str, arg);
    va_end (arg);

    printf ("%s", buf);
   }
}

void dump (int Verb, byte *buf, int len)
{
  dprintf (Verb, "_offset_|_0__1__2__3__4__5__6__7_|8__9__A__B__C__D__E__F_|______ASCII_____|\n");

  while (len)
   {
    dprintf (Verb, "%8X| ", buf);

    if (len >= 16)
     {
      dprintf (Verb, "%02X %02X %02X %02X %02X %02X %02X %02X|%02X %02X %02X %02X %02X %02X %02X %02X|",
      buf[0], buf[1], buf[2],  buf[3],  buf[4],  buf[5],  buf[6],  buf[7],
      buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);

      int count = 16;
      while (count--)
       {
        if (*buf > 0x20)
          dprintf (Verb, "%c", *buf);
        else
          dprintf (Verb, "%c", '.');

        buf++;
       }

      dprintf (Verb, "|\n");
      len -= 16;
     }
    else
     {
      int last = 16 - len;
      int count = 0, lastcount;

      while (len)
       {
        if (count != 7)
          dprintf (Verb, "%02X ", buf[count++]);
        else
          dprintf (Verb, "%02X|", buf[count++]);

        len--;
       }

      lastcount = count;

      while (last)
       {
        if (lastcount != 7 && lastcount != 15)
          dprintf (Verb, "   ");
        else
          dprintf (Verb, "  |");

        lastcount++;
        last--;
       }

      last = 16 - count + 1;
      lastcount = count - 1;

      while (count--)
       {
        if (*buf > 0x20)
          dprintf (Verb, "%c", *buf);
        else
          dprintf (Verb, "%c", '.');

        buf++;
       }

      while (last)
       {
        if (lastcount != 15)
          dprintf (Verb, " ");
        else
          dprintf (Verb, "|");

        lastcount++;
        last--;
       }

      dprintf (Verb, "\n");
     }
   }
}
