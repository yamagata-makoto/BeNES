/*
	mapper #2 support
	
	(c)makoto 2000
*/
#include "Mappers/Mapper002.h"
#include "NES/PatternTable.h"
#include "NES/ROM.h"

static unsigned CHRROM_UNIT = 0x2000;
static unsigned PRGROM_UNIT = 0x4000;

void Mapper002::init()
{
	PatternTable()->deepCopy8k(0x0000, prgrom.image);
	
	ROM()->copy16k(0x8000, prgrom.image+CHRROM_UNIT);
	ROM()->copy16k(0xc000, &(prgrom.image[prgrom.size-PRGROM_UNIT]));
}

void Mapper002::write(unsigned address, unsigned char data)
{
	unsigned pos = (unsigned)(data) * PRGROM_UNIT; 
	pos &= (prgrom.size-1);
	ROM()->copy16k(0x8000, &(prgrom.image[pos]));
}

