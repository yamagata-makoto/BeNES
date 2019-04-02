/*
	mapper #7 support
	
	(c)makoto 2000
*/
#include "Mappers/Mapper007.h"
#include "NES/ROM.h"
#include "NES/VRAM.h"

void Mapper007::write(unsigned address, unsigned char data)
{
	if (data & 0x10) {
		VRAM()->setMirrorType(MIRROR_1HIGH);
	}
	else {
		VRAM()->setMirrorType(MIRROR_1LOW);
	}
	unsigned pos = (unsigned)(data&0x0f) * 0x8000;
	ROM()->copy32k(0x8000, &(prgrom.image[pos]));
};

