static void dump (byte *buf, int len)
{
  printk (KERN_CRIT "_offset_|__0__1__2__3__4__5__6__7|_8__9__A__B__C__D__E__F|______ASCII_____|\n");

  while (len)
   {
    printk (KERN_CRIT "%8X| ", buf);

    if (len >= 16)
     {
      printk (KERN_CRIT "%02X %02X %02X %02X %02X %02X %02X %02X|%02X %02X %02X %02X %02X %02X %02X %02X|",
              buf[0], buf[1], buf[2],  buf[3],  buf[4],  buf[5],  buf[6],  buf[7], buf[8], buf[9], buf[10],
	      buf[11], buf[12], buf[13], buf[14], buf[15]);

      int count = 16;
      while (count--)
       {
        /*
        if (*buf >= 0x20)
          printk (KERN_CRIT "%c", *buf);
        else
          printk (KERN_CRIT "%c", '.');
         */
        buf++;
       }

      printk (KERN_CRIT "|\n");
      len -= 16;
     }
    else
     {
      int last = 16 - len;
      int count = 0, lastcount;

      while (len)
       {
        if (count != 7)
          printk (KERN_CRIT "%02X ", buf[count++]);
        else
          printk (KERN_CRIT "%02X|", buf[count++]);

        len--;
       }

      lastcount = count;

      while (last)
       {
        if (lastcount != 7 && lastcount != 15)
          printk (KERN_CRIT "   ");
        else
          printk (KERN_CRIT "  |");

        lastcount++;
        last--;
       }

      last = 16 - count + 1;
      lastcount = count - 1;

      while (count--)
       {
        /*
        if (*buf >= 0x20)
          printk (KERN_CRIT "%c", *buf);
        else
          printk (KERN_CRIT "%c", '.');
         */
        buf++;
       }

      while (last)
       {
        if (lastcount != 15)
          printk (KERN_CRIT " ");
        else
          printk (KERN_CRIT "|");

        lastcount++;
        last--;
       }

      printk (KERN_CRIT "\n");
     }
   }
}
