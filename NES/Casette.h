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
#ifndef Casette_H_DEFINED
#define Casette_H_DEFINED

#include <stdio.h>
#include "ROMfile.h"

class CasetteProtocol {
public:
	virtual void init() = 0;
	virtual void uninit() = 0;
	virtual int getMirrorType() = 0;
	virtual void write(unsigned address, unsigned char data) = 0;
	virtual void setROMdata(ROMfile* romfile) = 0;
	virtual void hblank(unsigned scanline) = 0;
};

class Casette: public CasetteProtocol
{
protected:
	ROMdata prgrom;
	ROMdata chrrom;
	ROMdata trainer;
	int     mirrorType;
	char    saveFile[1024];
public:
	Casette();	
	virtual ~Casette();
	virtual void setROMdata(ROMfile* romfile);
	virtual void init();
	virtual int getMirrorType() { return mirrorType; }
	virtual void write(unsigned address, unsigned char data);
	virtual void hblank(unsigned scanline) { ; }
	virtual void uninit();
};

#endif

