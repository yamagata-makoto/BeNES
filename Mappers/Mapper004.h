/*
	mapper #4 support
	
	(c)makoto 2000
*/
#ifndef Mapper004_H_DEFINED
#define Mapper004_H_DEFINED

#include "NES/Casette.h"

/* MMC3 */
class Mapper004: public Casette {

	bool prghigh;
	unsigned chrOffset;
	unsigned char command;
	unsigned prgsize;
	unsigned chrsize;

	unsigned irqLatch;
	unsigned irqCounter;
	bool     irqEnabled;
	bool     irqOccerEnabled;
	bool     irqSetMode;
	
	unsigned char* prgrom_ptr;
	unsigned char* chrrom_ptr;
	
	unsigned char regs[8];
protected:
	void write8000(unsigned char data);
	void write8001(unsigned char data);
	void writeA000(unsigned char data);
	void writeC000(unsigned char data);
	void writeC001(unsigned char data);
	void writeE000(unsigned char data);
	void writeE001(unsigned char data);
	void setbanks(unsigned char cmd, unsigned char data);
	void setbank_chr2k(unsigned addr, unsigned char data);
	void setbank_chr1k(unsigned addr, unsigned char data);
	void setbank_prgA(unsigned char data);
	void setbank_prgB(unsigned char data);

public:
	void init();
	void write(unsigned address, unsigned char data);
	void hblank(unsigned scanline);
};

#endif

