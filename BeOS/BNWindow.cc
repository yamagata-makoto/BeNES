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
#include <interface/Alert.h>
#include <interface/Screen.h>
#include <interface/View.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BeOS/BNApplication.h"
#include "BeOS/BNJoypad.h"
#include "BeOS/BNPreference.h"
#include "BeOS/BNWindow.h"
#include "BeOS/ColorPalette.h"
#include "NES/MapperFactory.h"
#include "NES/NES.h"
#include "NES/PaletteIndex.h"

#define X (100)
#define Y (100)
#define W (512)
#define H (480)

inline void
ShowMessage(char* msg)
{
  BAlert* alert = new BAlert(NULL, msg, "OK");
  alert->Go();
}

BNWindow::BNWindow()
  : BWindow(BRect(X, Y, X + W, Y + H),
            "BeNES",
            B_TITLED_WINDOW,
            B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE)
  , view(NULL)
{
  Lock();
  view = new BView(BRect(0, 0, W, H), NULL, B_FOLLOW_ALL, 0);
  AddChild(view);

  offscreen = new BBitmap(BRect(0, 0, W - 1, H - 1), B_COLOR_8_BIT);
  memset(offscreen->Bits(), 0, W * H);
  BScreen* screen = new BScreen(this);
  for (int i = 0; i < 64; i++) {
    colorIndex[i] = screen->IndexForColor(ColorPalette[i]);
  }
  delete screen;

  Unlock();
}

void
BNWindow::Show()
{
  be_app->PostMessage(BNMSG_BEGIN_EMULATE, be_app);

  static NESscreen screen;
  /* setup screen */
  screen.pitch = offscreen->BytesPerRow();
  screen.bits = (unsigned char*)(offscreen->Bits());

  memset(screen.bits, 0, W * H);

  PaletteIndex()->resetWith(colorIndex);

  NES::Instance().setScreen(&screen);
  NES::Instance().connect(this);

  BNJoypad::Instance()->startPolling();

  the_id =
    spawn_thread(BNWindow::threadFunc, "BeNES", B_NORMAL_PRIORITY, (void*)this);
  resume_thread(the_id);

  BWindow::Show();
}

void
BNWindow::updateScreen()
{
  static bigtime_t prev_tick = 0;
  static const bigtime_t frame_snooze = 1000000 / 60;
  Lock();
  view->DrawBitmap(offscreen);
  Unlock();
  bigtime_t snooze_time = frame_snooze - (system_time() - prev_tick);
  if (snooze_time > 0)
    snooze(snooze_time);
  prev_tick = system_time();
}

int32
BNWindow::threadFunc(void* arg)
{
  NES::Instance().run();
  return 0;
}

void
BNWindow::Quit()
{
  Lock();
  status_t result;

  NES::Instance().stop();
  wait_for_thread(the_id, &result);
  the_id = 0;
  printf("stoped\n");
  BNJoypad::Instance()->endPolling();

  BWindow::Hide();
  Unlock();

  be_app->PostMessage(BNMSG_END_EMULATE, be_app);
}
