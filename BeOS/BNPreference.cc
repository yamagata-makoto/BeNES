#include "BeOS/BNPreference.h"
#include <stdio.h>
#include <stdlib.h>
#include <storage/Directory.h>
#include <string.h>

BNPreference BNPreference::instance;

#define PREF_INPUTDEVICE "InputDevice"
#define PREF_FULLSCREEN "FullScreen"
#define PREF_ROMDIRECTORY "ROMDirectory"

#define GET_HOME() (getenv("HOME"))

inline void
readString(FILE* fp, char* buff, size_t size)
{
  fgets(buff, size, fp);
}

inline void
writeString(FILE* fp, char* buff)
{
  fputs(buff, fp);
}

inline int
readInteger(FILE* fp)
{
  char buff[BUFSIZ];
  readString(fp, buff, sizeof(buff));
  return atoi(buff);
}

inline void
writeInteger(FILE* fp, int val)
{
  char buff[BUFSIZ];
  sprintf(buff, "%d\n", val);
  writeString(fp, buff);
}

BNPreference::BNPreference()
  : inputDevice(DEVICE_KEYBOARD)
{
  char* p = GET_HOME();
  strcpy(prefDir, p);
  strcat(prefDir, "/config/settings/BeNES");
  BDirectory().CreateDirectory(prefDir, NULL);

  FILE* fp;
  char path[1024];
  // read input device
  strcpy(path, prefDir);
  strcat(path, "/" PREF_INPUTDEVICE);
  if ((fp = fopen(path, "r")) == NULL) {
    inputDevice = DEVICE_KEYBOARD;
  } else {
    inputDevice = readInteger(fp);
    fclose(fp);
  }
  // read fullscreen
  strcpy(path, prefDir);
  strcat(path, "/" PREF_FULLSCREEN);
  if ((fp = fopen(path, "r")) == NULL) {
    fullScreen = true;
  } else {
    fullScreen = (bool)(readInteger(fp));
    fclose(fp);
  }
  // read rom direcotry
  strcpy(path, prefDir);
  strcat(path, "/" PREF_ROMDIRECTORY);
  if ((fp = fopen(path, "r")) == NULL) {
    strcpy(romDir, GET_HOME());
  } else {
    memset(romDir, 0, sizeof(romDir));
    readString(fp, romDir, sizeof(romDir));
  }
}

BNPreference::~BNPreference()
{
  FILE* fp;
  char path[1024];

  // write input device
  strcpy(path, prefDir);
  strcat(path, "/" PREF_INPUTDEVICE);
  if ((fp = fopen(path, "w")) != NULL) {
    writeInteger(fp, inputDevice);
    fclose(fp);
  }
  // write fullscreen
  strcpy(path, prefDir);
  strcat(path, "/" PREF_FULLSCREEN);
  if ((fp = fopen(path, "w")) != NULL) {
    writeInteger(fp, (int)(fullScreen));
    fclose(fp);
  }
  // write rom directory
  strcpy(path, prefDir);
  strcat(path, "/" PREF_ROMDIRECTORY);
  if ((fp = fopen(path, "w")) != NULL) {
    writeString(fp, romDir);
    fclose(fp);
  }
}
