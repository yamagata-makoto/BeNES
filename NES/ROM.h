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
#ifndef ROM_H_DEFINED
#define ROM_H_DEFINED

class ROMprotocol {
	static ROMprotocol instance;
	ROMprotocol() { ; }
public:
	static ROMprotocol* Instance() { return &(ROMprotocol::instance); }
	void reset();
	void copy8k(unsigned address, unsigned char* data);
	void copy16k(unsigned address, unsigned char* data);
	void copy32k(unsigned address, unsigned char* data);
	void swap8k(unsigned addrA, unsigned addrB);
};

/* get ROM instance. */
ROMprotocol* ROM();

#endif
