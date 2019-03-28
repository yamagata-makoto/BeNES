/*
	mapper #6 support
	
	(c)makoto 2000
*/
#ifndef Mapper006_H_DEFINED
#define Mapper006_H_DEFINED

#include "Casette.h"
#include "CPU.h"

/* FFE F4xxx */
class Mapper006: public Casette,
                 public CPUwriteDelegate {
	bool irqEnabled;
	unsigned short irqCounter; 
public:
	void init();
	void write(unsigned address, unsigned char data);
	void write4_5(unsigned address, unsigned char data);
};

#endif

