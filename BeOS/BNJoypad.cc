/*
    BeNES - Nintendo Entertaiment System Emulator for BeOS
    
    * (C) 1999 makoto
    * (C) 1999 modified Jonathan Turner
    * (C) 2000 modified makoto

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
#include "BeOS/BNJoypad.h"
#include "NES/Joypad.h"
#include "BeOS/BNDefs.h"
#include <interface/InterfaceDefs.h>
#include <stdio.h>
#include <unistd.h>

BNJoypad BNJoypad::instance;

const int pollingRate = 1000000 / 300;

/*--------------------------------------------- BNKeyboardGrip ---*/
inline bool isKeyDown(key_info& info, unsigned char key)
{
	return (info.key_states[key>>3] & (1<<(7-(key&7))));
}

#define RETURN 0x47
#define SPACE 0x5e
#define UP 0x57
#define DOWN 0x62
#define LEFT 0x61
#define RIGHT 0x63
#define LSHIFT 0x4b
#define Z 0x4c
#define X 0x4d
#define LALT 0x5d
#define LCTRL 0x5c

void BNKeyboardGrip::get(JoypadProtocol* joy)
{
	static key_info info;
	
	get_key_info(&info);
	if (isKeyDown(info, UP)) joy->push(NESK_UP);
	else joy->release(NESK_UP);

	if (isKeyDown(info, DOWN)) joy->push(NESK_DOWN);
	else joy->release(NESK_DOWN);

	if (isKeyDown(info, LEFT)) joy->push(NESK_LEFT);
	else joy->release(NESK_LEFT);

	if (isKeyDown(info, RIGHT)) joy->push(NESK_RIGHT);
	else joy->release(NESK_RIGHT);

	if (isKeyDown(info, Z) || 
	    isKeyDown(info, LALT)) {
    	joy->push(NESK_A);
	}
	else joy->release(NESK_A);
		
	if (isKeyDown(info, X) || 
	    isKeyDown(info, LSHIFT) || 
	    isKeyDown(info, LCTRL)) {
		joy->push(NESK_B);
	}
	else joy->release(NESK_B);
	
	if (isKeyDown(info, RETURN)) joy->push(NESK_START);
	else joy->release(NESK_START);

	if (isKeyDown(info, SPACE)) joy->push(NESK_SELECT);
	else joy->release(NESK_SELECT);
}

/*---------------------------------------------- BNJoystickGrip ---*/
/* thanks! to Jonathan Turner */
#define PTOLERANCE ((int16)(25000))
#define NTOLERANCE ((int16)(-25000))

BNJoystickGrip::BNJoystickGrip(): isGamePadPro(false)
{
	static const char *PORTNAME="gameport/201";
	bjoy.Open(PORTNAME);
}

void BNJoystickGrip::get(JoypadProtocol* joy)
{
	static int32 buttonstat;
	static int16 axesstat[2];
	
	bjoy.Update();

	buttonstat = bjoy.ButtonValues();
	if (isGamePadPro) {
		if (buttonstat & 8) joy->push(NESK_A);
		else joy->release(NESK_A);
		
		if (buttonstat & 16) joy->push(NESK_B);
		else joy->release(NESK_B);
		
		if (buttonstat & 256) joy->push(NESK_START);
		else joy->release(NESK_START);
		
		if (buttonstat & 512) joy->push(NESK_SELECT);
		else joy->release(NESK_SELECT);
	}
	else {
		if (buttonstat & 0x02) joy->push(NESK_A);
		else joy->release(NESK_A);
		
		if (buttonstat & 0x01) joy->push(NESK_B);
		else joy->release(NESK_B);
		
		if (buttonstat & 0x04) joy->push(NESK_START);
		else joy->release(NESK_START);
		
		if (buttonstat & 0x08) joy->push(NESK_SELECT);
		else joy->release(NESK_SELECT);
	}	
	bjoy.GetAxisValues(axesstat);
	if (*(axesstat+1) > PTOLERANCE) joy->push(NESK_DOWN);
	else joy->release(NESK_DOWN);
		
	if (*(axesstat+1) < NTOLERANCE) joy->push(NESK_UP);
	else joy->release(NESK_UP);

	if (*(axesstat) > PTOLERANCE) joy->push(NESK_RIGHT);
	else joy->release(NESK_RIGHT);
	
	if (*(axesstat) < NTOLERANCE) joy->push(NESK_LEFT);
	else joy->release(NESK_LEFT);
}

/*---------------------------------------------------- BNJoypad ---*/
BNJoypad::BNJoypad(): id(0), running(false)
{
	keyboard = new BNKeyboardGrip;
	joystick = new BNJoystickGrip;
	grip = keyboard;
}

BNJoypad::~BNJoypad()
{
	endPolling();
	delete keyboard;
	delete joystick;
}

void BNJoypad::startPolling()
{
	if (id == 0) {
		running = true;
		id = spawn_thread(
			BNJoypad::Polling, 
			"BNJoypad", 
			B_NORMAL_PRIORITY, 
			this);
		resume_thread(id);
	}
}

void BNJoypad::endPolling()
{
	if (id) {
		status_t result;
		running = false;
		wait_for_thread(id, &result);
		kill_thread(id);
		id = 0;
	}
}

int32 BNJoypad::Polling(void* arg)
{
	JoypadProtocol* joyA = JoypadA();
	BNJoypad* obj = 
		reinterpret_cast<BNJoypad*>(arg);
	bigtime_t prev_tick = system_time();
	
	while (obj->running) {

		obj->grip->get(joyA);
		bigtime_t snooze_time = pollingRate - (system_time()-prev_tick);
		if (snooze_time > 0) snooze(snooze_time);
		prev_tick = system_time();
	}
	return 0;
}

void BNJoypad::setInputDeviceType(int type)
{
	switch (type) {
	case DEVICE_KEYBOARD:
		grip = keyboard;
		break;
	case DEVICE_JOYSTICK:
		joystick->setGamePadProGrip(false);
		grip = joystick;
		break;
	case DEVICE_GAMEPADPRO:
		joystick->setGamePadProGrip(true);
		grip = joystick;
		break;
	default:
		grip = keyboard;
		break;
	}
}

