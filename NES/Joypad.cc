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
#include "Joypad.h"

static void JOYAreset();
static unsigned char JOYAread();
static void JOYApush(unsigned char key);
static void JOYArelease(unsigned char key);

JoypadProtocol instanceA = {
	JOYAreset,
	JOYAread,
	JOYApush,
	JOYArelease
};

static unsigned char JOYAaccessBit;
static unsigned char JOYAdata;
static unsigned char JOYAreleaseLatch;
void JOYAreset()
{
	JOYAaccessBit = 0;
}

unsigned char JOYAread()
{
	unsigned char bit = ((JOYAdata>>JOYAaccessBit)&0x01);
	if ((JOYAreleaseLatch>>JOYAaccessBit) & 0x01) {
		unsigned char mask = ~(0x01<<JOYAaccessBit);
		JOYAdata &= mask;
		JOYAreleaseLatch &= mask;
	}
	JOYAaccessBit = (++JOYAaccessBit) & 0x07;
	return bit;
}

void JOYApush(unsigned char key)
{
	JOYAdata |= key;
}

void JOYArelease(unsigned char key)
{
	JOYAreleaseLatch |= key;
//	JOYAdata &= ~key;
}

JoypadProtocol* JoypadA()
{
	return &instanceA;
}

