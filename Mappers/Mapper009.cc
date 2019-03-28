/*
	mapper #9 support
	thanks to grugru

	(c)makoto 2000
*/
/* buggy */

#include "Mapper009.h"
#include "ROM.h"
#include "VRAM.h"
#include "PPU.h"
#include "PatternTable.h"

void Mapper009::init()
{
	data1.lo = data1.hi = data1.flag = 0;	
	data2.lo = data2.hi = data2.flag = 0;	
	PPU()->setDelegate(this);
	PatternTable()->copy8k(0x0000, chrrom.image);
	ROM()->copy8k(0x8000, prgrom.image);
	ROM()->copy8k(0xa000, &(prgrom.image[prgrom.size-(0x2000*3)]));
	ROM()->copy8k(0xc000, &(prgrom.image[prgrom.size-(0x2000*2)]));
	ROM()->copy8k(0xe000, &(prgrom.image[prgrom.size-(0x2000*1)]));
}

void Mapper009::rendering(unsigned address)
{
	unsigned pos;
	if (((address&0x3ff0)==0x0fd0) ||
	   (((address&0x3ff0)==0x1fd0))) {
		flag = 0;
		pos = (lo * 0x1000) & (chrrom.size-1);
		PatternTable()->copy4k(0x1000, &(chrrom.image[pos]));
	}
	else if (((address&0x3ff0)==0x0fe0) ||
	        (((address&0x3ff0)==0x1fe0))) {
		flag = 1;
		pos = (hi * 0x1000) & (chrrom.size-1);
		PatternTable()->copy4k(0x1000, &(chrrom.image[pos]));
	}
}

void Mapper009::write(unsigned address, unsigned char data)
{
	unsigned pos;
	switch (address & 0xf000) {
	case 0xa000:
		pos = (data * 0x2000) & (prgrom.size-1);	
		ROM()->copy8k(0x8000, &(prgrom.image[pos]));
		break;
	case 0xb000:
	case 0xc000:
		pos = (data * 0x1000) & (chrrom.size-1);
		PatternTable()->copy4k(0x0000, &(chrrom.image[pos]));
		break;
	case 0xd000:
		lo = data;
		if (flag == 0) {
			pos = (data * 0x1000) & (chrrom.size-1);
			PatternTable()->copy4k(0x1000, &(chrrom.image[pos]));
		}
		break;
	case 0xe000:
		hi = data;
		if (flag == 1) {
			pos = (data * 0x1000) & (chrrom.size-1);
			PatternTable()->copy4k(0x1000, &(chrrom.image[pos]));
		}
		break;
	case 0xf000:
		if (data & 0x01) {
			VRAM()->setMirrorType(MIRROR_HORIZONTAL);
		}
		else {
			VRAM()->setMirrorType(MIRROR_VERTICAL);
		}
		break;
	default:
		break;
	}
}

