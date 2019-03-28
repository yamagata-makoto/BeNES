/*
	mapper #4 support
	
	(c)makoto 2000
*/
#include "Mapper004.h"
#include "PatternTable.h"
#include "ROM.h"
#include "NES.h"
#include "CPU.h"
#include "VRAM.h"
#include "PPU.h"

static unsigned PRGROM_UNIT = 0x2000;
static unsigned CHRROM_UNIT = 0x400;

void Mapper004::hblank(unsigned scanline)
{
	irqSetMode = false;
	if (PPU()->isScreenVisible() && (scanline<240)) {
		irqCounter--;
	}
	if (irqCounter <= 0) {
		if (irqEnabled && irqOccerEnabled) {
			CPU()->intIRQ();
			irqOccerEnabled = false;
		}
		irqSetMode = true;
		irqCounter = irqLatch;
	}
}

void Mapper004::init()
{
	prghigh = false;
	chrOffset = 0;
	command = 0;

	irqLatch = 0;
	irqCounter = 0;;
	irqEnabled = false;
	irqOccerEnabled = false;
	irqSetMode = false;

	for (int i=0; i<8; i++) {
		regs[i] = 0;
	}
	if (chrrom.size > 0) {
		prgrom_ptr = prgrom.image;
		chrrom_ptr = chrrom.image;
		prgsize = prgrom.size;
		chrsize = chrrom.size;
		ROM()->copy16k(0x8000, prgrom_ptr);
		ROM()->copy16k(0xc000, &(prgrom_ptr[prgsize-0x4000]));
		PatternTable()->copy8k(0x0000, chrrom_ptr);
	}
	else {
		prgrom_ptr = prgrom.image;
		chrrom_ptr = prgrom_ptr;
		prgsize = prgrom.size;
		chrsize = prgsize;
		ROM()->copy16k(0x8000, &(prgrom_ptr[0x2000]));
		ROM()->copy16k(0xc000, &(prgrom_ptr[prgsize-0x4000]));
		PatternTable()->deepCopy8k(0x0000, chrrom_ptr);
	}
}

void Mapper004::write8000(unsigned char data)
{
	chrOffset = 0x0000;
	if (data & 0x80) {
		chrOffset = 0x1000;
	}
	prghigh = ((data & 0x40) != 0);	
	if ((command&0x80) != (data&0x80)) {
		for (int i=0; i<6; i++) {
			setbanks(i, regs[i]);
		}
	}
	if ((command&0x40) != (data&0x40)) {
		for (int i=6; i<8; i++) {
			setbanks(i, regs[i]);
		}
	}
	command = data;
}

void Mapper004::write8001(unsigned char data)
{
	setbanks(command, data);
}

void Mapper004::writeA000(unsigned char data)
{
	if (data & 0x40) {
		VRAM()->setMirrorType(MIRROR_1HIGH);
	}
	else if (data & 0x01) {
		VRAM()->setMirrorType(MIRROR_HORIZONTAL);
	}
	else {
		VRAM()->setMirrorType(MIRROR_VERTICAL);
	}
}

void Mapper004::writeC000(unsigned char data)
{
	irqLatch = data;
	if (irqSetMode) {
		irqCounter = irqLatch;
	}
}

void Mapper004::writeC001(unsigned char data)
{
	irqSetMode = true;
	irqCounter = data;
}

void Mapper004::writeE000(unsigned char data)
{
	irqEnabled = false;
}

void Mapper004::writeE001(unsigned char data)
{
	irqEnabled = true;
	irqOccerEnabled = true;
}

void Mapper004::write(unsigned address, unsigned char data)
{
	switch (address & 0xe001) {
	case 0x8000: write8000(data); break;
	case 0x8001: write8001(data); break;
	case 0xa000: writeA000(data); break;
	case 0xc000: writeC000(data); break;
	case 0xc001: writeC001(data); break;
	case 0xe000: writeE000(data); break;
	case 0xe001: writeE001(data); break;
	}
}

void Mapper004::setbank_chr2k(unsigned addr, unsigned char data)
{
	unsigned vram_offset = 0;
	unsigned vrom_offset = 0;
	vram_offset = addr^chrOffset;
	data &= 0xfe;
	vrom_offset = data * CHRROM_UNIT;
	vrom_offset &= (chrsize-1);
	PatternTable()->copy2k(vram_offset, &(chrrom_ptr[vrom_offset]));
}

void Mapper004::setbank_chr1k(unsigned addr, unsigned char data)
{
	unsigned vram_offset = 0;
	unsigned vrom_offset = 0;
	vram_offset = addr^chrOffset;
	vrom_offset = data * CHRROM_UNIT;
	vrom_offset &= (chrsize-1);

	PatternTable()->copy1k(vram_offset, &(chrrom_ptr[vrom_offset]));
}

void Mapper004::setbank_prgA(unsigned char data)
{
	unsigned prom_offset = 0;

	prom_offset = data * PRGROM_UNIT;
	prom_offset &= (prgsize-1);
	if (prghigh) {
		ROM()->copy8k(0xc000, &(prgrom_ptr[prom_offset]));
		ROM()->copy8k(0x8000, &(prgrom_ptr[prgsize-0x4000]));
	}
	else {
		ROM()->copy8k(0x8000, &(prgrom_ptr[prom_offset]));
		ROM()->copy8k(0xc000, &(prgrom_ptr[prgsize-0x4000]));
	}
}

void Mapper004::setbank_prgB(unsigned char data)
{
	unsigned prom_offset = 0;

	prom_offset = data * PRGROM_UNIT;
	prom_offset &= (prgsize-1);
	ROM()->copy8k(0xa000, &(prgrom_ptr[prom_offset]));
	if (prghigh) {
		ROM()->copy8k(0x8000, &(prgrom_ptr[prgsize-0x4000]));
	}
	else {
		ROM()->copy8k(0xc000, &(prgrom_ptr[prgsize-0x4000]));
	}
}

void Mapper004::setbanks(unsigned char cmd, unsigned char data)
{
	regs[cmd&0x07] = data; 
	switch (cmd & 0x07) {
	case 0: setbank_chr2k(0x0000, data); break;
	case 1: setbank_chr2k(0x0800, data); break;
	case 2: setbank_chr1k(0x1000, data); break;
	case 3: setbank_chr1k(0x1400, data); break;
	case 4: setbank_chr1k(0x1800, data); break;
	case 5: setbank_chr1k(0x1c00, data); break;
	case 6: setbank_prgA(data); break;
	case 7: setbank_prgB(data); break;
	}
}

