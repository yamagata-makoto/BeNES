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
#include "NES/PatternTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
PTreset();
static void
PTcopy1k(unsigned address, unsigned char* data);
static void
PTcopy2k(unsigned address, unsigned char* data);
static void
PTcopy4k(unsigned address, unsigned char* data);
static void
PTcopy8k(unsigned address, unsigned char* data);
static void
PTdeepCopy8k(unsigned address, unsigned char* data);

static PatternTableProtocol instance = { PTreset,  PTcopy1k, PTcopy2k,
                                         PTcopy4k, PTcopy8k, PTdeepCopy8k };

static unsigned char defaultPatternTable[0x2000];

BOOL PatternTableWriteProtect;

unsigned char* patternTable[8];
PatternTile cachedTileA[0x100];
PatternTile cachedTileB[0x100];

static void
copy(unsigned address, unsigned char* data, unsigned num);

void
PTreset()
{
  int i;
  int d;

  memset(cachedTileA, 0, sizeof(cachedTileA));
  memset(cachedTileB, 0, sizeof(cachedTileB));
  for (d = 0; d < 0x100; d++) {
    for (i = 0; i < 8; i++) {
      cachedTileA[d].data[7 - i] = ((d >> i) & 0x01);
      cachedTileB[d].data[7 - i] = ((d >> i) & 0x01) << 1;
    }
  }
  memset(defaultPatternTable, 0, sizeof(defaultPatternTable));
  for (i = 0; i < 8; i++) {
    patternTable[i] = &(defaultPatternTable[i * 0x400]);
  }
  PatternTableWriteProtect = YES;
}

void
PTdeepCopy8k(unsigned address, unsigned char* data)
{
  int i;

  memcpy(defaultPatternTable, data, 0x2000);
  for (i = 0; i < 8; i++) {
    patternTable[i] = &(defaultPatternTable[i * 0x400]);
  }
  PatternTableWriteProtect = NO;
}

void
PTcopy1k(unsigned address, unsigned char* data)
{
  copy(address, data, 1);
}

void
PTcopy2k(unsigned address, unsigned char* data)
{
  copy(address, data, 2);
}

void
PTcopy4k(unsigned address, unsigned char* data)
{
  copy(address, data, 4);
}

void
PTcopy8k(unsigned address, unsigned char* data)
{
  copy(address, data, 8);
}

void
copy(unsigned address, unsigned char* data, unsigned num)
{
  if (PatternTableWriteProtect == NO)
    return;

  unsigned pos = address >> 10;
  unsigned ofs = 0x0000;
  unsigned i;

  for (i = 0; i < num; i++, pos++, ofs += 0x0400) {
    patternTable[pos] = (data + ofs);
  }
  PatternTableWriteProtect = YES;
}

PatternTableProtocol*
PatternTable()
{
  return &instance;
}
