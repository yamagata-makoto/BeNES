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
#include "SEALAPU/SEALAPU.h"
#include "SEALAPU/NesAPU.h"

static NesAPU* apu = NULL;

static void writeSq0(unsigned address, unsigned char data);
static void writeSq1(unsigned address, unsigned char data);
static void writeTri(unsigned address, unsigned char data);
static void writeNiz(unsigned address, unsigned char data);

typedef void (*writeFuncPtr)(unsigned address, unsigned char data);

writeFuncPtr writeFuncs[16] = {
	writeSq0, writeSq0, writeSq0, writeSq0,
	writeSq1, writeSq1, writeSq1, writeSq1,
	writeTri, writeTri, writeTri, writeTri,
	writeNiz, writeNiz, writeNiz, writeNiz
};

void SEALAPUreset()
{
	if (apu == NULL) {
		apu = new NesAPU;
	}
	apu->reset();
}

unsigned char SEALAPUread(unsigned address)
{
	unsigned char ret = 0x00;
	if (address == 0x4015) ret = apu->read4015(); 
	return ret;
}

void SEALAPUwrite(unsigned address, unsigned char data)
{
	if (address <= 0x400f) {
		(*writeFuncs[address&0x0f])(address, data);
	}
	else if (address == 0x4015) {
		apu->write4015(data);
	}
}

void SEALAPUstatusChange()
{
	apu->statusChange();
}

void SEALAPUstop()
{
	apu->stop();
	apu->reset();
}

//====================================================================
void writeSq0(unsigned address, unsigned char data)
{
	apu->writeSq0(address-0x4000, data);
}

void writeSq1(unsigned address, unsigned char data)
{
	apu->writeSq1(address-0x4004, data);
}

void writeTri(unsigned address, unsigned char data)
{
	apu->writeTri(address-0x4008, data);
}

void writeNiz(unsigned address, unsigned char data)
{
	apu->writeNiz(address-0x400c, data);
}

