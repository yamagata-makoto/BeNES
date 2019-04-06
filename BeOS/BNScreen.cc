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
#include <app/Application.h>
#include <interface/View.h>
#include <interface/Screen.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>

#include "BeOS/BNScreen.h"
#include "BeOS/ColorPalette.h"
#include "BeOS/BNJoypad.h"
#include "BeOS/BNPreference.h"
#include "NES/NES.h"
#include "NES/PaletteIndex.h"
#include "NES/MapperFactory.h"

static status_t err;

inline bool isKeyDown(key_info& info, unsigned char key)
{
	return (info.key_states[key>>3] & (1<<(7-(key&7))));
}

BNScreen::BNScreen():
	BWindowScreen("BeNES", B_8_BIT_640x480, &err),
	view(NULL)
{
	if (err != B_OK) be_app->PostMessage(B_QUIT_REQUESTED, be_app);
	Lock();
	view = new BView(BRect(0, 0, 9, 9), NULL, B_FOLLOW_ALL, 0);
	AddChild(view);
	Unlock();
}

void BNScreen::ScreenConnected(bool connected)
{
	static BOOL first = YES;
	static NESscreen screen;

	if (connected) {
printf("screen connected!\n");
		if ((SetSpace(B_8_BIT_640x480)<B_OK) || (SetFrameBuffer(640, 2048)<B_OK)) {
			printf("error\n");
			be_app->PostMessage(B_QUIT_REQUESTED, be_app);
		}
		first = NO;
		/* setup screen */
		PaletteIndex()->reset();
		SetColorList(ColorPalette, 0, 63);
		screen.pitch = CardInfo()->bytes_per_row;
		screen.bits = (unsigned char*)CardInfo()->frame_buffer;
		memset(screen.bits, 63, 2048*screen.pitch);
		screen.bits += 64;
		NES::Instance().setScreen(&screen);
		
		NES::Instance().connect(this);
		BNJoypad::Instance()->startPolling();
		thread_id id = spawn_thread(
		BNScreen::threadFunc, "BeNES", B_NORMAL_PRIORITY, (void*)this);
		resume_thread(id);
	}
	else {
printf("screen disconnected!\n");
		NES::Instance().stop();
		BNJoypad::Instance()->endPolling();
	}
}

void BNScreen::updateScreen()
{
	static bigtime_t prev_tick = 0;
	static const bigtime_t frame_snooze = 1000000 / 60;
	bigtime_t snooze_time = frame_snooze - (system_time()-prev_tick);
	if (snooze_time > 0) snooze(snooze_time);
	prev_tick = system_time();
}

int32 BNScreen::threadFunc(void* arg)
{
	NES::Instance().run();
	return 0;
}

bool BNScreen::QuitRequested()
{
	Hide();
	return false;
}
