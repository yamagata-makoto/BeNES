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
#include <stdio.h>
#include <string.h>
#include "Defs.h"
#include "VRAM.h"
#include "PatternTable.h"

static void VRAMreset();
static void VRAMsetMirrorType(int mirrorType);

static VRAMprotocol instance = {
	VRAMreset,
	VRAMsetMirrorType
};

static unsigned char nameTableMemory[0x1000];
static unsigned char cachedAttrTableMemory[0x1000];

static void vramHorizontalMirror();
static void vramVerticalMirror();
static void vram1HighMirror();
static void vram1LowMirror();
static void vram4ScreenMirror();

unsigned char* nameTable[4];
unsigned char* cachedAttrTable[4];

void VRAMreset()
{
	memset(nameTableMemory, 0, sizeof(nameTableMemory));	
	memset(cachedAttrTableMemory, 0, sizeof(cachedAttrTableMemory));	

	nameTable[0] = (nameTableMemory+0x000);
	nameTable[1] = (nameTableMemory+0x000);
	nameTable[2] = (nameTableMemory+0x400);
	nameTable[3] = (nameTableMemory+0x400);

	cachedAttrTable[0] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[1] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[2] = (cachedAttrTableMemory+0x400);
	cachedAttrTable[3] = (cachedAttrTableMemory+0x400);

}

void VRAMsetMirrorType(int mirrorType)
{
	switch (mirrorType) {
	case MIRROR_HORIZONTAL: 
		vramHorizontalMirror();
		break;
	case MIRROR_VERTICAL:
		vramVerticalMirror();
		break;
	case MIRROR_1HIGH:
		vram1HighMirror();
		break;
	case MIRROR_1LOW:
		vram1LowMirror();
		break;
	case MIRROR_4SCREEN:
		vram4ScreenMirror();
		break;
	default:
		break; 
	}
}

VRAMprotocol* VRAM()
{
	return &instance;
}

void vramHorizontalMirror()
{
	printf("setting horizontal mirror...");
	nameTable[0] = (nameTableMemory+0x000);
	nameTable[1] = (nameTableMemory+0x000);
	nameTable[2] = (nameTableMemory+0x400);
	nameTable[3] = (nameTableMemory+0x400);
	cachedAttrTable[0] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[1] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[2] = (cachedAttrTableMemory+0x400);
	cachedAttrTable[3] = (cachedAttrTableMemory+0x400);
	printf("done.\n");
}

void vramVerticalMirror()
{
	printf("setting vertical mirror...");
	nameTable[0] = (nameTableMemory+0x000);
	nameTable[1] = (nameTableMemory+0x400);
	nameTable[2] = (nameTableMemory+0x000);
	nameTable[3] = (nameTableMemory+0x400);
	cachedAttrTable[0] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[1] = (cachedAttrTableMemory+0x400);
	cachedAttrTable[2] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[3] = (cachedAttrTableMemory+0x400);
	printf("done.\n");
}

void vram1HighMirror()
{
	printf("setting 1high mirror...");
	nameTable[0] = (nameTableMemory+0x400);
	nameTable[1] = (nameTableMemory+0x400);
	nameTable[2] = (nameTableMemory+0x400);
	nameTable[3] = (nameTableMemory+0x400);
	cachedAttrTable[0] = (cachedAttrTableMemory+0x400);
	cachedAttrTable[1] = (cachedAttrTableMemory+0x400);
	cachedAttrTable[2] = (cachedAttrTableMemory+0x400);
	cachedAttrTable[3] = (cachedAttrTableMemory+0x400);
	printf("done.\n");
}

void vram1LowMirror()
{
	printf("setting 1low mirror...");
	nameTable[0] = (nameTableMemory+0x000);
	nameTable[1] = (nameTableMemory+0x000);
	nameTable[2] = (nameTableMemory+0x000);
	nameTable[3] = (nameTableMemory+0x000);
	cachedAttrTable[0] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[1] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[2] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[3] = (cachedAttrTableMemory+0x000);
	printf("done.\n");
}

void vram4ScreenMirror()
{
	printf("setting 4screen mirror...");
	nameTable[0] = (nameTableMemory+0x000);
	nameTable[1] = (nameTableMemory+0x400);
	nameTable[2] = (nameTableMemory+0x800);
	nameTable[3] = (nameTableMemory+0xc00);
	cachedAttrTable[0] = (cachedAttrTableMemory+0x000);
	cachedAttrTable[1] = (cachedAttrTableMemory+0x400);
	cachedAttrTable[2] = (cachedAttrTableMemory+0x800);
	cachedAttrTable[3] = (cachedAttrTableMemory+0xc00);
	printf("done.\n");
}
