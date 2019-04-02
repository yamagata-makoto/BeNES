/*
    BeNES - Nintendo Entertaiment System Emulator for BeOS
    
    * (C) 2000 by makoto yamagata

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include "M6502/M6502.h"
#include "NES/Joypad.h"
#include "NES/CPU.h"
#include "NES/NES.h"
#include "NES/PPU.h"
#include "NES/APU.h"
#include "SEALAPU/SEALAPU.h"

M6502 core;
CPUprotocol CPUprotocol::instance;
static CPUwriteDelegate* writeDelegate = NULL;
static CPUreadDelegate* readDelegate = NULL;

static unsigned char reg4015 = 0x00;
static unsigned char reg4016 = 0x00;
static unsigned char reg4017 = 0x00;

/* from ROM interface */
extern unsigned char* rom[4];

/* from RAM interface */
extern unsigned char* ram;

/* from WRAM interface */
extern unsigned char* wram;

unsigned char RAMread(unsigned address);
unsigned char PPUread(unsigned address);
unsigned char ROMread(unsigned address);
unsigned char CPUread(unsigned address);
unsigned char WRAMread(unsigned address);

void RAMwrite(unsigned address, unsigned char data);
void PPUwrite(unsigned address, unsigned char data);
void NESwrite(unsigned address, unsigned char data);
void CPUwrite(unsigned address, unsigned char data);
void WRAMwrite(unsigned address, unsigned char data);

//const int CPU_FRAME_CYCLES = (29870);
//const int CPU_UNIT_CYCLES = (CPU_FRAME_CYCLES / 4);
//const int CPU_FRAME_CYCLES = (113*258);
//const int CPU_UNIT_CYCLES = (CPU_FRAME_CYCLES / 4);
const int CPU_UNIT_CYCLES = 7457;
const int CPU_FRAME_CYCLES = CPU_UNIT_CYCLES * 4;

static int frameClock = 0;
static int ticks = 0;

static void countCycles(int cycles);

void CPUprotocol::reset()
{
	memset(&core, 0, sizeof(M6502));
	
	core.RamBank[0] = &ram;    /* $0000-$1FFF */ 
	core.RamBank[1] = NULL;    /* $2000-$3FFF */
	core.RamBank[2] = NULL;    /* $4000-$5FFF */
	core.RamBank[3] = &wram;   /* $6000-$7FFF */
	core.RamBank[4] = &rom[0]; /* $8000-$9FFF */
	core.RamBank[5] = &rom[1]; /* $A000-$BFFF */
	core.RamBank[6] = &rom[2]; /* $C000-$DFFF */
	core.RamBank[7] = &rom[3]; /* $E000-$FFFF */

	core.RdFuncs[0] = RAMread; /* $0000-$1FFF */ 
	core.RdFuncs[1] = PPUread; /* $2000-$3FFF */
	core.RdFuncs[2] = CPUread; /* $4000-$5FFF */
	core.RdFuncs[3] = WRAMread;/* $6000-$7FFF */
	core.RdFuncs[4] = ROMread; /* $8000-$9FFF */
	core.RdFuncs[5] = ROMread; /* $A000-$BFFF */
	core.RdFuncs[6] = ROMread; /* $C000-$DFFF */
	core.RdFuncs[7] = ROMread; /* $E000-$FFFF */

	core.WrFuncs[0] = RAMwrite; /* $0000-$1FFF */ 
	core.WrFuncs[1] = PPUwrite; /* $2000-$3FFF */
	core.WrFuncs[2] = CPUwrite; /* $4000-$5FFF */
	core.WrFuncs[3] = WRAMwrite;/* $6000-$7FFF */
	core.WrFuncs[4] = NESwrite; /* $8000-$9FFF */
	core.WrFuncs[5] = NESwrite; /* $A000-$BFFF */
	core.WrFuncs[6] = NESwrite; /* $C000-$DFFF */
	core.WrFuncs[7] = NESwrite; /* $E000-$FFFF */
	
	APU()->reset();
	frameClock = 0;
	ticks = 0;
	Reset6502(&core);
}

void CPUprotocol::run(int cycles)
{
	core.IPeriod = cycles;
	Run6502(&core);
	countCycles(cycles);
}

void CPUprotocol::setDelegate(CPUwriteDelegate* aDelegate)
{
	writeDelegate = aDelegate;
}

void CPUprotocol::setDelegate(CPUreadDelegate* aDelegate)
{
	readDelegate = aDelegate;
}

void CPUprotocol::intNMI()
{
//	core.IRequest = INT_NMI;
	Int6502(&core, INT_NMI);
}

void CPUprotocol::intIRQ()
{
//	core.IRequest = INT_IRQ;
	Int6502(&core, INT_IRQ);
}

unsigned char CPUread(unsigned address)
{
	unsigned char ret = 0x00;

	if (address == 0x4015) {
		reg4015 &= ~0x0f;
		reg4015 |= (APU()->read(0x4015)&0x0f);
		ret = reg4015;
		reg4015 &= ~0x40;
//		frameClock = 0;
//		core.IRequest = INT_NONE;
	}
	else if (address == 0x4016) {
		reg4016 = JoypadA()->read();
		ret = reg4016;
	}
	else if (address == 0x4017) {
		ret = 0x40;
	}
	else if (readDelegate) {
		ret = readDelegate->read4_5(address);
	}
	return ret;
}

void CPUwrite(unsigned address, unsigned char data)
{
	if (address <= 0x400f) {
		APU()->write(address, data);
	}
	else if (address == 0x4014) {
		PPU()->spriteDMA(ram+(((unsigned)(data))*0x100));
	}
	else if (address == 0x4015) {
		reg4015 = data;
		APU()->write(0x4015, data);
		if (data & 0x10) {
			reg4015 &= ~0x80;
		}
	}
	else if (address == 0x4016) {
		if ((reg4016&0x01) && (data==0x00)) {
			JoypadA()->reset();
		}
		reg4016 = data;
	}
	else if (address == 0x4017) {
		reg4017 = data;
		frameClock = 0;
//		core.IRequest = INT_NONE;
//		reg4015 &= ~0x40;
	}
	else if (writeDelegate) {
		writeDelegate->write4_5(address, data);
	}
}

void countCycles(int cycles)
{
	cycles = (core.IPeriod-core.ICount);
	frameClock += cycles;
	if ((frameClock>CPU_FRAME_CYCLES) && !(reg4017&0xc0)) { 
		frameClock %= CPU_FRAME_CYCLES;
		CPU()->intIRQ();
		reg4015 |= 0x40;
	}
	ticks += cycles;
	if (ticks > CPU_UNIT_CYCLES) {
		ticks %= CPU_UNIT_CYCLES;
		APU()->statusChange();
	}
}

CPUprotocol* CPU()
{
	return CPUprotocol::Instance();
}

void Wr6502(register word Addr,register byte Value)
{
	(*core.WrFuncs[Addr>>13])(Addr, Value);
}

byte Rd6502(register word Addr)
{
	return (*core.RdFuncs[Addr>>13])(Addr);
}

byte Op6502(register word Addr)
{	
	unsigned char** bank = core.RamBank[Addr>>13];
	unsigned char ret = 0x00;
	if (bank) {
		ret = (*bank)[Addr&0x1fff];
	}
	return ret;
}
