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
#ifndef PaletteIndex_H_DEFINED
#define PaletteIndex_H_DEFINED

class PaletteIndexProtocol {
	static PaletteIndexProtocol instance;
	PaletteIndexProtocol() { reset(); }
public:
	static PaletteIndexProtocol* Instance() { return &instance; }
	void reset();
	void resetWith(unsigned char* index);
	unsigned char toIndex(unsigned char c);
};

PaletteIndexProtocol* PaletteIndex();

#endif