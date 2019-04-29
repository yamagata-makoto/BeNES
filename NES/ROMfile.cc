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
#include "NES/ROMfile.h"
#include "NES/Defs.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tagHeader
{
  unsigned char id[4];
  unsigned char prg_banks;
  unsigned char chr_banks;
  unsigned char rom_info1;
  unsigned char rom_info2;
  unsigned char reserve[8];
} ROMfileHeader;

#define IS_VERTICAL_MIRROR(header) ((header.rom_info1 & 0x01) ? YES : NO)
#define IS_4SCREEN_MIRROR(header) ((header.rom_info1 & 0x08) ? YES : NO)
#define HAS_SAVERAM(header) ((header.rom_info1 & 0x02) ? YES : NO)
#define HAS_TRAINER(header) ((header.rom_info1 & 0x04) ? YES : NO)

/* private functions */
static unsigned getMapperNumber(ROMfileHeader* header);
static unsigned char* readROM(unsigned size, FILE* fp);
static void deepCopyROMdata(ROMdata* to, ROMdata* from);

ROMfile::ROMfile()
{
  prgrom.image = NULL;
  prgrom.size = 0;
  chrrom.image = NULL;
  chrrom.size = 0;
  trainer.image = NULL;
  trainer.size = 0;
  mapperNumber = 0;
  mirrorType = MIRROR_HORIZONTAL;
  fp = NULL;
}

BOOL
ROMfile::open(const char* file, char* err)
{
  ROMfileHeader header;
  BOOL reply = NO;

  strcpy(fileName, file);
  if (fp)
    this->close();
  if ((fp = fopen(file, "rb")) != NULL) {
    fread(&header, 1, sizeof(ROMfileHeader), fp);
    if (memcmp(header.id, "NES\x1a", 4) != 0) {
      strcpy(err, "unknown rom format.\n");
      fclose(fp);
      return NO;
    }
    mapperNumber = ::getMapperNumber(&header);
    if (IS_VERTICAL_MIRROR(header) == YES) {
      mirrorType = MIRROR_VERTICAL;
    } else if (IS_4SCREEN_MIRROR(header) == YES) {
      mirrorType = MIRROR_4SCREEN;
    } else {
      mirrorType = MIRROR_HORIZONTAL;
    }
    if (HAS_TRAINER(header)) {
      printf("found trainer\n");
      trainer.size = NES_TRAINER_SIZE;
      trainer.image = readROM(trainer.size, fp);
    }
    flagSaveRAM = NO;
    if (HAS_SAVERAM(header)) {
      flagSaveRAM = YES;
    }
    if (header.prg_banks) {
      printf("found PRG-ROM...");
      prgrom.banks = header.prg_banks;
      prgrom.size = header.prg_banks * NES_PRGBANK_SIZE;
      prgrom.image = readROM(prgrom.size, fp);
      printf("%dkb\n", prgrom.size / 1024);
    }
    if (header.chr_banks) {
      printf("found CHR-ROM...");
      chrrom.banks = header.chr_banks;
      chrrom.size = header.chr_banks * NES_CHRBANK_SIZE;
      chrrom.image = readROM(chrrom.size, fp);
      printf("%dkb\n", chrrom.size / 1024);
    }
    reply = YES;
  } else {
    sprintf(err, "cant open file(%s)", file);
  }
  return reply;
}

void
ROMfile::close()
{
  fclose(fp);
  if (prgrom.image)
    free(prgrom.image);
  prgrom.image = NULL;
  prgrom.size = 0;
  if (chrrom.image)
    free(chrrom.image);
  chrrom.image = NULL;
  chrrom.size = 0;
  if (trainer.image)
    free(trainer.image);
  trainer.image = NULL;
  trainer.size = 0;

  mapperNumber = 0;
  mirrorType = MIRROR_HORIZONTAL;
}

void
ROMfile::getPrgrom(ROMdata* data)
{
  deepCopyROMdata(data, &prgrom);
}

void
ROMfile::getChrrom(ROMdata* data)
{
  deepCopyROMdata(data, &chrrom);
}

void
ROMfile::getTrainer(ROMdata* data)
{
  deepCopyROMdata(data, &trainer);
}

unsigned
ROMfile::getMapperNumber()
{
  return mapperNumber;
}

int
ROMfile::getMirrorType()
{
  return mirrorType;
}

unsigned
getMapperNumber(ROMfileHeader* header)
{
  unsigned mapper = (unsigned)((header->rom_info1 >> 4) & 0x0f);
  if (((header->rom_info2 & 0x0f) == 0) && (header->reserve[0] == 0)) {
    mapper |= (header->rom_info2 & 0xf0);
  }
  return mapper;
}

unsigned char*
readROM(unsigned size, FILE* fp)
{
  unsigned char* image = NULL;
  if (size) {
    image = (unsigned char*)malloc(size);
    if (image) {
      fread(image, 1, size, fp);
    }
  }
  return image;
}

void
deepCopyROMdata(ROMdata* to, ROMdata* from)
{
  to->image = NULL;
  to->size = 0;
  to->banks = 0;
  if (from->size) {
    to->image = (unsigned char*)malloc(from->size);
    memcpy(to->image, from->image, from->size);
    to->size = from->size;
    to->banks = from->banks;
  }
}
