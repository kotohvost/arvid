#include <stdio.h>

#include "mytypes.h"
#include "adriver.h"
#include "manchstr.h"

// Hardware parametrs
dword HArvFrame = HFRAME_1031, WArvFrame = WFRAME_1031;

// Logical parametrs
dword HeadStr = HEADER_STR;
dword HDataFrame = HArvFrame - HeadStr, WDataFrame = WArvFrame << 1;

void ErrorExit (dword NumError)
{
  switch (NumError)
   {
    case 1:  printf ("Memory allocation error!!!\n"); break;
    case 2:  printf ("Found restore error!!!\n");     break;
    case 3:  printf ("Not found file!!!\n");	      break;
    default: printf ("Unknown Error!!!\n");	      break;
   }

  exit (NumError);
}

void TestSpeedTable ()
{
  for (dword i = 0; i < 0x100; i++)
   {
    word TWord = SpeedLowTabl [i];
    dword j = SpeedHighTabl [TWord & 0xAAAA] & 0xFF;

    if (i != j)
      printf ("Src byte: %Xh, dst byte %Xh\n", i, j);
   }
}

int main (void)
{
  printf ("Manchester coder/encoder test program.\n");
  InitManchTable ();
  TestSpeedTable ();
  FreeManchTable ();
  return 0;
}
