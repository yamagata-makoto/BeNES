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
#include "NES/NES.h"
#include "NES/ROM.h"
#include "NES/CPU.h"
#include "NES/RAM.h"
#include "NES/PPU.h"
#include "NES/APU.h"
#include "NES/VRAM.h"
#include "NES/WRAM.h"
#include "NES/Joypad.h"

#define SCREEN_W     (256)
#define SCREEN_H     (240)
#define SCREEN_DEPTH (8)

NES NES::instance;

static NESscreen*       screen = NULL;
static CasetteProtocol* mapper = NULL;
static UpdateListener*  updateListener = NULL;

inline void executeCPU1scanline(unsigned scanline);

void NESwrite(unsigned address, unsigned char data)
{
	mapper->write(address, data);
}

void NES::insertCasette(CasetteProtocol* obj)
{
	mapper = obj;
}

void NES::connect(UpdateListener* listener)
{
	updateListener = listener;
}

void NES::reset()
{
	JoypadA()->reset();
	
	/* clear memory */
	RAM()->clear();
	
	/* reset H/W */
	PPU()->reset();
	ROM()->reset();

	/* set vram mirror */
	VRAM()->setMirrorType(mapper->getMirrorType());
	
	/* mapper init */
	CPU()->setDelegate((CPUwriteDelegate*)NULL);
	CPU()->setDelegate((CPUreadDelegate*)NULL);
	PPU()->setDelegate(NULL);
	mapper->init();

	/* reset CPU */
	CPU()->reset();
	
}

void NES::run()
{
	running = YES;

	this->reset();

	scanline = 0;
	while (running == YES) {
		executeCPU1scanline(scanline);
		PPU()->drawScanline(scanline, screen);
		if (scanline++ == 241) {
			updateListener->updateScreen();
			PPU()->setVBLANK();
			CPU()->run(8);
			if (PPU()->isExecuteNMIonVBLANK() == YES) {
				CPU()->intNMI();
			}
			for (; scanline<=261; scanline++) {
				executeCPU1scanline(scanline);
			}
			PPU()->resetVBLANK();
			scanline = 0;
			CPU()->run(80);
		}
	}
	mapper->uninit();
	APU()->stop();
}

void NES::setScreen(NESscreen* obj)
{
	screen = obj;
}

void NES::stop()
{
	running = NO;
}


NES& NES::Instance()
{
	return instance;
}

/*------------------------------------------- private functions ---*/
void executeCPU1scanline(unsigned scanline)
{
	CPU()->run(113);
	mapper->hblank(scanline);
}
