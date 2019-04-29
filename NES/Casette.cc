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
#include "NES/Casette.h"
#include "NES/PatternTable.h"
#include "NES/ROM.h"
#include "NES/VRAM.h"
#include "NES/WRAM.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Casette::Casette()
{
  prgrom.image = NULL;
  prgrom.size = 0;
  chrrom.image = NULL;
  chrrom.size = 0;
  trainer.image = NULL;
  trainer.size = 0;
  saveFile[0] = '\0';
}

Casette::~Casette()
{
  if (prgrom.image)
    free(prgrom.image);
  if (chrrom.image)
    free(chrrom.image);
  if (trainer.image)
    free(trainer.image);
}

void
Casette::setROMdata(ROMfile* romfile)
{
  if (prgrom.image)
    free(prgrom.image);
  if (chrrom.image)
    free(chrrom.image);
  if (trainer.image)
    free(trainer.image);

  saveFile[0] = '\0';

  romfile->getPrgrom(&prgrom);
  romfile->getChrrom(&chrrom);
  romfile->getTrainer(&trainer);
  mirrorType = romfile->getMirrorType();

  WRAM()->clear();
  if (trainer.size > 0) {
    WRAM()->copyTrainer(trainer.image);
  }
  if (romfile->hasSaveRAM() == YES) {
    char buff[1024];
    strcpy(buff, romfile->getFileName());
    char* p = strrchr(buff, '.');
    strcpy(p, ".sav");
    strcpy(saveFile, buff);
    WRAM()->loadFromFile(buff);
  }
}

void
Casette::uninit()
{
  if (saveFile[0]) {
    WRAM()->saveToFile(saveFile);
  }
}

void
Casette::init()
{
  /* copy prgrom */
  if (prgrom.size == 0x4000) { /* 16K ROM */
    ROM()->copy16k(0x8000, prgrom.image);
    ROM()->copy16k(0xC000, prgrom.image);
  } else {
    ROM()->copy16k(0x8000, prgrom.image);
    ROM()->copy16k(0xc000, &(prgrom.image[prgrom.size - 0x4000]));
  }
  /* copy chrrom */
  if (chrrom.size > 0) { /* CHR-RAM */
    PatternTable()->copy8k(0x0000, chrrom.image);
  }
}

void
Casette::write(unsigned addresss, unsigned char data)
{}
