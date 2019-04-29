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
#include "NES/ROM.h"
#include <stdio.h>

ROMprotocol ROMprotocol::instance;

unsigned char* rom[4];

static void copy(unsigned address, unsigned char* data, unsigned num);

void
ROMprotocol::reset()
{
  int i;
  for (i = 0; i < 4; i++) {
    rom[i] = NULL;
  }
}

void
ROMprotocol::swap8k(unsigned addrA, unsigned addrB)
{
  unsigned posA = (addrA >> 13) & 0x03;
  unsigned posB = (addrB >> 13) & 0x03;
  unsigned char* tmp = rom[posA];
  rom[posA] = rom[posB];
  rom[posB] = tmp;
}

void
ROMprotocol::copy8k(unsigned address, unsigned char* data)
{
  copy(address, data, 1);
}

void
ROMprotocol::copy16k(unsigned address, unsigned char* data)
{
  copy(address, data, 2);
}

void
ROMprotocol::copy32k(unsigned address, unsigned char* data)
{
  copy(address, data, 4);
}

void
copy(unsigned address, unsigned char* data, unsigned num)
{
  unsigned pos = (address >> 13) & 0x03;
  unsigned ofs = 0x0000;
  unsigned i;

  for (i = 0; i < num; i++, pos++, ofs += 0x2000) {
    rom[pos] = (data + ofs);
  }
}

ROMprotocol*
ROM()
{
  return ROMprotocol::Instance();
}

unsigned char
ROMread(unsigned address)
{
  return rom[(address >> 13) & 0x03][address & 0x1fff];
}
