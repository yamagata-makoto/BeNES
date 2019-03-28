/*
	mapper #1 support
	
	(c)makoto 2000
*/
#ifndef Mapper001_H_DEFINED
#define Mapper001_H_DEFINED

#include "Casette.h"

/* MMC1 */
class Mapper001: public Casette {
	unsigned char regs[4];
	unsigned char counter;
	unsigned char latch;
	int           carttype;
	unsigned char* prgrom_ptr;
	unsigned char* chrrom_ptr;
	unsigned prgsize;
	unsigned chrsize;
public:
	void init();
	void write(unsigned address, unsigned char data);
	int calcBankbase();
	void setCHRbank();
	void setPRGbank();
	void setMirror();
};

#endif

