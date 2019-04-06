/*
	mapper #34 support
	thanks to grugru

	(c)makoto 2000
	
*/
#include "Mapper034.h"
#include "NES/PatternTable.h"
#include "NES/ROM.h"

void Mapper034::init()
{
	PatternTable()->deepCopy8k(0x0000, chrrom.image);
	
	ROM()->copy16k(0x8000, prgrom.image);
	ROM()->copy16k(0xc000, &(prgrom.image[prgrom.size-0x4000]));
}

void Mapper034::write(unsigned address, unsigned char data)
{
	/* buggy */
	unsigned pos = data * 0x8000;
	pos &= (prgrom.size-1);
	ROM()->copy32k(0x8000, &(prgrom.image[pos]));
}

