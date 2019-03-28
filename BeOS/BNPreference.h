#ifndef BNPreference_H_DEFINED
#define BNPreference_H_DEFINED

#include <string.h>
#include "BNDefs.h"

class BNPanel;
class BNApplication;

class BNPreference {
friend BNPanel;
friend BNApplication;
	static BNPreference instance;
	char prefDir[1024];

	int inputDevice;
	bool fullScreen;
	char romDir[1024];
public:
	static BNPreference* Instance() { return &instance; }
	int getInputDevice() { return inputDevice; }
	bool isFullScreen() { return fullScreen; }
	const char* getROMdirectory() { return romDir; }
private:
	BNPreference();
	~BNPreference();
	void setInputDevice(int val) { inputDevice = val; }
	void setFullScreen(bool val) { fullScreen  = val; }
	void setROMdirectory(const char* dir) { strcpy(romDir, dir); }
};

#endif
