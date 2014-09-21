
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "inifile.h"

typedef struct
{
  csIniFile *ini;
  char *Section;
} EnumStruc, *pEnumStruc;

bool EnumComments (csSome Parm, char *Text)
{
  (void) Parm;
  if (Text)
    printf ("#%s\n", Text);
  else
    printf ("\n");

  return (false);
}

bool EnumData (csSome cur, char *Name, size_t DataSize, csSome Data)
{
  (void) DataSize;

  pEnumStruc (cur)->ini->EnumComments (pEnumStruc (cur)->Section, Name, EnumComments, NULL);

  printf ("%s = %s\n", Name, (char *) Data);
  return (false);
}

bool EnumSec (csSome p_this, char *Name)
{
  EnumStruc s = {(csIniFile *) p_this, Name};

  s.ini->EnumComments (Name, NULL, EnumComments, NULL);

  printf ("[%s]\n", Name);
  ((csIniFile *)p_this)->EnumData (Name, EnumData, &s);

  return (false);
}

int main (int argc, char* argv[])
{
  csIniFile ini (';');

  printf ("Load = %d\n", ini.Load ("test.ini"));
  printf ("section.key = [%s]\n", ini.GetStr ("section", "key"));
  printf ("another section.binary data = [%s]\n", ini.GetStr ("another section", "binary data"));

  ini.SetStr ("section", "key", "new value for key");
  ini.SetStr ("section", "another key", "new value for another key");

  bool sectionexists = ini.SectionExists ("another section");
  ini.SetStr ("another section", "yet another key", "new value for yet another key in another section");
  ini.SetComment ("another section", "yet another key", "*yet another comment :-)*");

  if (!sectionexists)
  {
    ini.SetComment ("another section", NULL, "*-----===============-----*");
    ini.SetComment ("another section", NULL, "*     !!!! w o w !!!!	*");
    ini.SetComment ("another section", NULL, "*-----===============-----*");
  }

  printf ("section.key = [%s]\n", ini.GetStr ("section", "key"));

  ini.EnumSections(NULL, EnumSec, (csSome)&ini);

  char bin[600];

  for (int i = 0; i < 600; i++)
    bin [i] = rand ();

  ini.SetData ("another section", "binary data", bin, sizeof(bin));
  printf ("Save = %d\n", ini.Save ("test.ini"));

  return 0;
}
