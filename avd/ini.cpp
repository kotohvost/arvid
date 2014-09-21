
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "inifile.h"
#include "dprint.h"
#include "arvid.h"
#include "vcr.h"
#include "ini.h"

/* Global variables */
/* trains finding of frame, frame not found timeout */
int retrain_count = 3, timeout = 15;

/* parameters from ini file for each board and VHS */
avdinipar avdpar [MAX_AVT];

int LoadIniFile (char *ini_file, avdinipar *pai, int boards)
{
  csIniFile ini (';');

  if (!ini.Load (ini_file))
    return false;
    
  retrain_count = ini.GetInt ("common", "retrain_count", 3);
  timeout       = ini.GetInt ("common", "timeout", 15);
  VerbFlag      = ini.GetInt ("common", "verbose", 5);
  DFlag         = ini.GetInt ("common", "debug", 1);

  char section [MAX_PATH];
  for (int i = 0; i < boards; i++, pai++)
   {
    memset (pai, 0, sizeof (avdinipar));
    sprintf (section, "board%d", i);
    
    pai->insert     = ini.GetYesNo (section, "insert", false);
    pai->phase      = ini.GetInt   (section, "phase", 3);
    pai->velosity   = ini.GetFloat (section, "velosity", 0.02346);
    pai->sub_phase  = ini.GetInt   (section, "sub_phase", 1);
    pai->auto_phase = ini.GetYesNo (section, "auto_phase", true);
    pai->auto_power = ini.GetYesNo (section, "auto_power", true);

    strcpy (pai->sp_vcr_filename,  ini.GetStr (section, "sp_vcr_filename"));
    strcpy (pai->lp_vcr_filename,  ini.GetStr (section, "lp_vcr_filename"));
    strcpy (pai->elp_vcr_filename, ini.GetStr (section, "elp_vcr_filename"));
   }

  return true;
}

int SaveIniFile (char *ini_file, avdinipar *pai, int boards)
{
  csIniFile ini (';');
  
  if (!ini.Load (ini_file))
    return 0;
    
  ini.SetInt     ("common", "retrain_count", retrain_count);
  ini.SetComment ("common", "retrain_count", ASV);
  ini.SetInt     ("common", "timeout", timeout);
  ini.SetComment ("common", "timeout", ASV);
  ini.SetInt     ("common", "verbose", VerbFlag);
  ini.SetComment ("common", "verbose", ASV);
  ini.SetInt     ("common", "debug", DFlag);
  ini.SetComment ("common", "debug", ASV);
    
  char section [MAX_PATH];
  for (int i = 0; i < boards; i++, pai++)
   {
    sprintf (section, "board%d", i);

    if (pai->insert)
      ini.SetStr   (section, "insert", "true");
    else
      ini.SetStr   (section, "insert", "false");
    
    ini.SetComment (section, "insert", ASV);

    ini.SetInt     (section, "phase", pai->phase);
    ini.SetComment (section, "phase", ASV);

    ini.SetFloat   (section, "velosity", pai->velosity);
    ini.SetComment (section, "velosity", ASV);
    
    ini.SetInt     (section, "sub_phase", pai->sub_phase);
    ini.SetComment (section, "sub_phase", ASV);
    
    if (pai->auto_phase)
      ini.SetStr   (section, "auto_phase", "yes");
    else
      ini.SetStr   (section, "auto_phase", "no");

    ini.SetComment (section, "auto_phase", ASV);
    
    if (pai->auto_power)
      ini.SetStr   (section, "auto_power", "yes");
    else
      ini.SetStr   (section, "auto_power", "no");

    ini.SetComment (section, "auto_power", ASV);

    ini.SetStr     (section, "sp_vcr_filename", pai->sp_vcr_filename);
    ini.SetComment (section, "sp_vcr_filename", ASV);
    ini.SetStr     (section, "lp_vcr_filename", pai->lp_vcr_filename);
    ini.SetComment (section, "lp_vcr_filename", ASV);
    ini.SetStr     (section, "elp_vcr_filename", pai->elp_vcr_filename);
    ini.SetComment (section, "elp_vcr_filename", ASV);
   }

  if (!ini.Save (ini_file))
    return false;

  return true;
}
