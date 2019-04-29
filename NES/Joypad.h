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
#ifndef Joypad_H_DEFINED
#define Joypad_H_DEFINED

#ifdef __cplusplus
extern "C"
{
#endif

#define NESK_A (0x01)
#define NESK_B (0x02)
#define NESK_SELECT (0x04)
#define NESK_START (0x08)
#define NESK_UP (0x10)
#define NESK_DOWN (0x20)
#define NESK_LEFT (0x40)
#define NESK_RIGHT (0x80)

typedef struct
{
  void (*reset)();
  unsigned char (*read)();
  void (*push)(unsigned char key);
  void (*release)(unsigned char key);
} JoypadProtocol;

JoypadProtocol* JoypadA();

#ifdef __cplusplus
}
#endif

#endif
