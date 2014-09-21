/*
 * читаем имя файла из входной строки
 */

#include "stdio.h"

/* загрузка конфиг файла */
#if 0
Config		Cfg;
#else
char		*CfgDev = "/dev/ravt0";
char		*CfgTdr = "/usr/local/lib/arvid/tdr";
char		*CfgAvt = "/usr/local/lib/arvid/avt";
char		*CfgVcrd = "/usr/local/lib/arvid";
char		*CfgVcr = "akair110.vcr";
char		*CfgLog = "/var/log/arvid/arvid.log";
char		*CfgErr = "/var/log/arvid/arvid.err";
char		CfgPhase = 0;
char		CfgPhase3 = 17;
char		CfgTime = 13;
#endif
