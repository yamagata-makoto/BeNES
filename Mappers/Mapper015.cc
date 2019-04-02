/*
	mapper #15 support
	
	(c)makoto 2000
*/
/* not works yet. */

#include "Mappers/Mapper015.h"
#include "NES/ROM.h"
#include "NES/VRAM.h"
#include "NES/PatternTable.h"

void Mapper015::init()
{
	PatternTable()->deepCopy8k(0x0000, prgrom.image);
	ROM()->copy32k(0x8000, &(prgrom.image[0]));
}

void Mapper015::write(unsigned address, unsigned char data)
{
	/* buggy */
	unsigned bank = (data & 0x3f) << 1;
	switch (address) {
	case 0x8000:
		if (data & 0x40) {
			VRAM()->setMirrorType(MIRROR_HORIZONTAL);
		}
		else {
			VRAM()->setMirrorType(MIRROR_VERTICAL);
		}
		/*
		if (data & 0x80) {
			ROM()->swap8k(0x8000, 0xa000);
			ROM()->swap8k(0xc000, 0xe000);
		}
		*/
		ROM()->copy8k(0x8000, &prgrom.image[(bank+0)*0x2000]);
		ROM()->copy8k(0xa000, &prgrom.image[(bank+1)*0x2000]);
		ROM()->copy8k(0xc000, &prgrom.image[(bank+2)*0x2000]);
		ROM()->copy8k(0xe000, &prgrom.image[(bank+3)*0x2000]);
		break;
	case 0x8001:
		/*
		if (data & 0x80) {
			ROM()->swap8k(0xc000, 0xe000);
		}
		*/
		ROM()->copy8k(0xc000, &prgrom.image[(bank+0)*0x2000]);
		ROM()->copy8k(0xe000, &prgrom.image[(bank+1)*0x2000]);
		break;
	case 0x8002:
		bank += (data & 0x80)? 1: 0;
		ROM()->copy8k(0x8000, &prgrom.image[(bank+0)*0x2000]);
		ROM()->copy8k(0xa000, &prgrom.image[(bank+0)*0x2000]);
		ROM()->copy8k(0xc000, &prgrom.image[(bank+0)*0x2000]);
		ROM()->copy8k(0xe000, &prgrom.image[(bank+0)*0x2000]);
		break;
	case 0x8003:
		if (data & 0x40) {
			VRAM()->setMirrorType(MIRROR_HORIZONTAL);
		}
		else {
			VRAM()->setMirrorType(MIRROR_VERTICAL);
		}
		ROM()->copy8k(0xc000, &prgrom.image[(bank+0)*0x2000]);
		ROM()->copy8k(0xe000, &prgrom.image[(bank+1)*0x2000]);
		break;
	}
}

