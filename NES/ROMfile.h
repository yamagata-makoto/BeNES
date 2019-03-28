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
#ifndef ROMFile_H_DEFINED
#define ROMFile_H_DEFINED

#include <stdio.h>
#include "Defs.h"

#define NES_TRAINER_SIZE (0x200)
#define NES_SAVERAM_SIZE (0x2000)
#define NES_PRGBANK_SIZE (0x4000)
#define NES_CHRBANK_SIZE (0x2000)

typedef struct {
	unsigned char* image;
	unsigned       size;
	unsigned       banks;
} ROMdata;

class ROMfile
{
	ROMdata  prgrom;
	ROMdata  chrrom;
	ROMdata  trainer;
	unsigned mapperNumber;
	int      mirrorType;
	FILE*    fp;
	char     fileName[1024];
	BOOL     flagSaveRAM;
public:
	ROMfile();
	BOOL open(const char* file, char* err);
	void close();
	void getPrgrom(ROMdata* data);
	void getChrrom(ROMdata* data);
	void getTrainer(ROMdata* data);
	unsigned getMapperNumber();
	const char* getFileName() { return fileName; }
	BOOL hasSaveRAM() { return flagSaveRAM; }
	int getMirrorType();
};

#endif

