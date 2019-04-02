/*
	mapper #6 support
	
	(c)makoto 2000
*/
/* not works yet. */

#include "Mappers/Mapper006.h"
#include "NES/PatternTable.h"
#include "NES/ROM.h"
#include "NES/VRAM.h"

void Mapper006::init()
{
	CPU()->setDelegate(this);
	Casette::init();
}

void Mapper006::write(unsigned address, unsigned char data)
{
	/* buggy */
	unsigned prgpos = (data>>2) * 0x4000;
	unsigned chrpos = (data&0x03) * 0x2000;
	switch (address & 0xe000) {
	case 0x8000:
	case 0xA000:
	case 0xC000:
	case 0xE000:
		ROM()->copy16k(0x8000, &(prgrom.image[prgpos]));		
		PatternTable()->copy8k(0x0000, &(chrrom.image[chrpos]));		
		break;
	default:
		break;
	}
}

void Mapper006::write4_5(unsigned address, unsigned char data)
{
	switch (address) {
	case 0x42fc:
	case 0x42fd:
		break;
	case 0x42fe:
		if (data & 0x10) {
			VRAM()->setMirrorType(MIRROR_1LOW);
		}
		else {
			VRAM()->setMirrorType(MIRROR_1HIGH);
		}
		break;
	case 0x42ff:
		if (data & 0x10) {
			VRAM()->setMirrorType(MIRROR_HORIZONTAL);
		}
		else {
			VRAM()->setMirrorType(MIRROR_VERTICAL);
		}
		break;
	case 0x43fe:
		break;
	case 0x4500:
		break;
	case 0x4501:
		irqEnabled = false;
		break;
	case 0x4502:
		irqCounter &= 0xff00;
		irqCounter |= data;
		break;
	case 0x4503:
		irqCounter &= 0x00ff;
		irqCounter |= ((unsigned)(data)<<8) & 0xff00;
		break;
	default:
		break;
	}
}

