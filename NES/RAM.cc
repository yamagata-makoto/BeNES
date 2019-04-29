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
#include "NES/RAM.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RAMprotocol RAMprotocol::instance;

static unsigned char memory[0x2000];
unsigned char* ram = memory;

unsigned char RAMread(unsigned address);
void RAMwrite(unsigned address, unsigned char data);

RAMprotocol*
RAM()
{
  return RAMprotocol::Instance();
}

void
RAMprotocol::clear()
{
  memset(memory, 0, sizeof(memory));
}

unsigned char
RAMread(unsigned address)
{
  return memory[address & 0x7ff];
}

void
RAMwrite(unsigned address, unsigned char data)
{
  memory[(address & 0x7ff) ^ 0x0000] = 
  memory[(address & 0x7ff) ^ 0x0800] =
  memory[(address & 0x7ff) ^ 0x1000] = 
  memory[(address & 0x7ff) ^ 0x1800] = data;
}
