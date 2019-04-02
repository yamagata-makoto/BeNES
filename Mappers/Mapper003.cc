/*
	mapper #3 support
	
	(c)makoto 2000
*/
#include "Mappers/Mapper003.h"
#include "NES/PatternTable.h"

static unsigned CHRROM_UNIT = 0x2000;

void Mapper003::write(unsigned address, unsigned char data)
{
	unsigned pos = (unsigned)(data) * CHRROM_UNIT; 
	pos &= (chrrom.size-1);
	PatternTable()->copy8k(0x0000, &(chrrom.image[pos]));
};

