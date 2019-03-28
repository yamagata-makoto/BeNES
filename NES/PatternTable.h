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
#ifndef PetternTable_H_DEFINED
#define PetternTable_H_DEFINED

#include "Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned char data[8];
} PatternTile;

typedef struct { 
	void (*reset)();
	void (*copy1k)(unsigned address, unsigned char* data);
	void (*copy2k)(unsigned address, unsigned char* data);
	void (*copy4k)(unsigned address, unsigned char* data);
	void (*copy8k)(unsigned address, unsigned char* data);
	void (*deepCopy8k)(unsigned address, unsigned char* data);
} PatternTableProtocol;

/* get PatternTable instance. */
PatternTableProtocol* PatternTable();

#ifdef __cplusplus
}
#endif

#endif
