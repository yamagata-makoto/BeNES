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
#ifndef APU_H_DEFINED
#define APU_H_DEFINED

#include "NES/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PPU protocol */
typedef struct {
	void (*reset)();
	void (*write)(unsigned address, unsigned char data);
	unsigned char (*read)(unsigned address);
	void (*statusChange)();
	void (*stop)();
} APUprotocol;

/* get PPU instance.(Singleton pattern) */
APUprotocol* APU();

#ifdef __cplusplus
}
#endif

#endif
