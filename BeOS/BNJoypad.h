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
#ifndef BNJOYPAD_H_DEFINED
#define BNJOYPAD_H_DEFINED

#include <device/Joystick.h>
#include <kernel/OS.h>
#include "Joypad.h"

class BNGrip {
public:
	BNGrip() { ; }
	virtual ~BNGrip() { ; }
	virtual void get(JoypadProtocol* joy) = 0;
};

class BNKeyboardGrip: public BNGrip {
public:
	void get(JoypadProtocol* joy);
};

class BNJoystickGrip: public BNGrip {
	BJoystick bjoy;
	bool isGamePadPro;
public:
	BNJoystickGrip();
	void get(JoypadProtocol* joy);
	void setGamePadProGrip(bool flag) { isGamePadPro = flag; }
};

class BNJoypad {
static BNJoypad instance;

	thread_id id;
	bool running;
	BNKeyboardGrip* keyboard;
	BNJoystickGrip* joystick;
	BNGrip* grip;
	
	
	BNJoypad();
	~BNJoypad();

	static int32 Polling(void* arg);
public:
	static BNJoypad* Instance() { return &instance; }
	
	void setInputDeviceType(int type);
	void startPolling();
	void endPolling();
	
};

#endif
