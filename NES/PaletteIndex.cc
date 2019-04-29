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
#include "NES/PaletteIndex.h"

PaletteIndexProtocol PaletteIndexProtocol::instance;

unsigned char index[64];

void
PaletteIndexProtocol::reset()
{
  for (int i = 0; i < 64; i++) {
    index[i] = i;
  }
}

void
PaletteIndexProtocol::resetWith(unsigned char* aIndex)
{
  for (int i = 0; i < 64; i++) {
    index[i] = aIndex[i];
  }
}

unsigned char
PaletteIndexProtocol::toIndex(unsigned char data)
{
  return index[data];
}

PaletteIndexProtocol*
PaletteIndex()
{
  return PaletteIndexProtocol::Instance();
}
