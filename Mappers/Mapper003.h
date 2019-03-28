/*
	mapper #3 support
	
	(c)makoto 2000
*/
#ifndef Mapper003_H_DEFINED
#define Mapper003_H_DEFINED

#include "Casette.h"

/* CNROM */
class Mapper003: public Casette {
public:
	void write(unsigned address, unsigned char data);
};

#endif

