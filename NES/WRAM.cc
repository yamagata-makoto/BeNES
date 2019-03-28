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
#include <stdlib.h>
#include <string.h>
#include "WRAM.h"

static void WRAMclear();
static BOOL WRAMsaveToFile(const char* fileName);
static BOOL WRAMloadFromFile(const char* fileName);
static void WRAMcopyTrainer(unsigned char* data);

static WRAMprotocol instance = {
	WRAMclear,
	WRAMsaveToFile,
	WRAMloadFromFile,
	WRAMcopyTrainer
};

static unsigned char memory[0x2000];
unsigned char* wram = memory;

unsigned char WRAMread(unsigned address);
void WRAMwrite(unsigned address, unsigned char data);

void WRAMclear()
{
	memset(memory, 0, sizeof(memory));	
}

unsigned char WRAMread(unsigned address)
{
	return memory[address&0x1fff];
}

void WRAMwrite(unsigned address, unsigned char data)
{
	memory[address&0x1fff] = data;
}

BOOL WRAMsaveToFile(const char* fileName)
{
	FILE* fp = fopen(fileName, "wb");
	BOOL reply = NO;
	if (fp) {
		reply = YES;
		fwrite(memory, 1, sizeof(memory), fp);
		fclose(fp);
	}

	return reply;
}

BOOL WRAMloadFromFile(const char* fileName)
{
	FILE* fp = fopen(fileName, "rb");
	BOOL reply = NO;
	if (fp) {
		reply = YES;
		fread(memory, 1, sizeof(memory), fp);
		fclose(fp);
	}
	return reply;
}

void WRAMcopyTrainer(unsigned char* data)
{
	memcpy(&(memory[0x1000]), data, 0x200);
}

WRAMprotocol* WRAM()
{
	return &instance;
}
