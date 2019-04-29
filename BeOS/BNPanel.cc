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
#include <interface/Box.h>
#include <interface/Button.h>
#include <interface/CheckBox.h>
#include <interface/RadioButton.h>
#include <interface/View.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BeOS/BNJoypad.h"
#include "BeOS/BNPanel.h"
#include "BeOS/BNPreference.h"

/* message */
#define BNM_EMULATE 'emul'
#define BNM_KEYBOARD 'kybd'
#define BNM_JOYSTICK 'jyst'
#define BNM_GAMEPADPRO 'gpro'
#define BNM_FULLSCREEN 'fscr'

/* main window frame size */
const BRect frame(100, 100, 360 - 1, 235 - 1);

class BNPanelView : public BView
{
public:
  BNPanelView(const BRect& r);
};

BNPanelView::BNPanelView(const BRect& r)
  : BView(r, "BeNESPanelView", B_FOLLOW_ALL, B_WILL_DRAW)
{
  BNPreference* pref = BNPreference::Instance();

  BNJoypad* joy = BNJoypad::Instance();
  joy->setInputDeviceType(pref->getInputDevice());

  BBox* box = new BBox(BRect(10, 10, 120 - 1, 120 - 1), "InputDevBox");
  box->SetLabel("Input Device");
  BRadioButton* radio = new BRadioButton(BRect(10, 25, 100 - 1, 50 - 1),
                                         "keyboard_radio",
                                         "Keyboard",
                                         new BMessage(BNM_KEYBOARD));
  if (pref->getInputDevice() == DEVICE_KEYBOARD) {
    radio->SetValue(B_CONTROL_ON);
  }
  box->AddChild(radio);

  radio = new BRadioButton(BRect(10, 50, 100 - 1, 75 - 1),
                           "joystick_radio",
                           "Joystick",
                           new BMessage(BNM_JOYSTICK));
  if (pref->getInputDevice() == DEVICE_JOYSTICK) {
    radio->SetValue(B_CONTROL_ON);
  }
  box->AddChild(radio);

  radio = new BRadioButton(BRect(10, 75, 100 - 1, 100 - 1),
                           "gamepadpro_radio",
                           "Gamepad Pro",
                           new BMessage(BNM_GAMEPADPRO));
  if (pref->getInputDevice() == DEVICE_GAMEPADPRO) {
    radio->SetValue(B_CONTROL_ON);
  }
  box->AddChild(radio);
  AddChild(box);

  BButton* button = NULL;
  button = new BButton(BRect(140, 20, 235 - 1, 50 - 1),
                       BUTTON_EMULATE,
                       "Emulate...",
                       new BMessage(BNM_EMULATE));
  button->MakeDefault(true);
  AddChild(button);

  BCheckBox* check = new BCheckBox(BRect(140, 58, 235 - 1, 78 - 1),
                                   CHECKBOX_FULLSCREEN,
                                   "Full screen",
                                   new BMessage(BNM_FULLSCREEN));
  if (pref->isFullScreen()) {
    check->SetValue(B_CONTROL_ON);
  }
  AddChild(check);
}

BNPanel::BNPanel()
  : BWindow(frame, "BeNES: Control Panel", B_TITLED_WINDOW, B_NOT_RESIZABLE)
{
  Lock();
  BRect r = frame;
  r.OffsetTo(B_ORIGIN);
  BView* view = new BNPanelView(r);
  AddChild(view);

  openPanel = new BFilePanel;
  openPanel->SetPanelDirectory(BNPreference::Instance()->getROMdirectory());
  Unlock();
}

bool
BNPanel::QuitRequested()
{
  be_app->PostMessage(B_QUIT_REQUESTED, be_app);
  return true;
}

void
BNPanel::MessageReceived(BMessage* msg)
{
  Lock();
  BNPreference* pref = BNPreference::Instance();
  BNJoypad* joy = BNJoypad::Instance();
  BCheckBox* check = (BCheckBox*)(FindView(CHECKBOX_FULLSCREEN));
  switch (msg->what) {
    case BNM_EMULATE:
      openPanel->Show();
      break;
    case BNM_KEYBOARD:
      pref->setInputDevice(DEVICE_KEYBOARD);
      joy->setInputDeviceType(DEVICE_KEYBOARD);
      break;
    case BNM_JOYSTICK:
      pref->setInputDevice(DEVICE_JOYSTICK);
      joy->setInputDeviceType(DEVICE_JOYSTICK);
      break;
    case BNM_GAMEPADPRO:
      pref->setInputDevice(DEVICE_GAMEPADPRO);
      joy->setInputDeviceType(DEVICE_GAMEPADPRO);
      break;
    case BNM_FULLSCREEN:
      pref->setFullScreen(check->Value() == B_CONTROL_ON);
      break;
    default:
      BWindow::MessageReceived(msg);
      break;
  }
  Unlock();
}
