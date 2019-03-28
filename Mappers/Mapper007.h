/*
	mapper #7 support
	
	(c)makoto 2000
*/
#ifndef Mapper007_H_DEFINED
#define Mapper007_H_DEFINED

#include "Casette.h"

/* AOROM */
class Mapper007: public Casette {
public:
	void write(unsigned address, unsigned char data);
};

#endif

