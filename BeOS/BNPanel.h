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
#ifndef BNPanel_H_DEFINED
#define BNPanel_H_DEFINED

#include <interface/Window.h>
#include <interface/Bitmap.h>
#include <interface/View.h>
#include <storage/FilePanel.h>

/* control name */
#define BUTTON_EMULATE "emulateButton"
#define CHECKBOX_FULLSCREEN "fullCheck"

class BNPanel: public BWindow
{
	BFilePanel* openPanel;
public:
	BNPanel();
	bool QuitRequested();
	void MessageReceived(BMessage* msg);
};

#endif
