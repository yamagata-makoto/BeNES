/*
** BeNES - Nintendo Entertaiment System Emulator for BeOS
**
** * 1999 by makoto yamagata
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
//
// Audio Processo Unit (APU)
//
#ifndef NESAPU_H_DEFINED
#define NESAPU_H_DEFINED

#include "NesSound.h"

class NesAPU {
	NesSound mSound;
	unsigned char mSq0Regs[4];
	unsigned char mSq1Regs[4];
	unsigned char mTriRegs[4];
	unsigned char mNizRegs[4];
	unsigned char mVolume[4];
	int mFadeCounter[4];
	int mFadeValue[4];
	int mLength[4];
	int mTriLen;
	int mSweepCounter[2];
	int mSweepValue[2];
	unsigned char mSweepSpeed[2];
	unsigned char mSweepDirection[2];
	unsigned char m4015;
	unsigned char mToggle;
	bool mSoundEnable;
	unsigned char mTriUpdate;
public:
	NesAPU(): m4015(0x0f), mToggle(0), mSoundEnable(true) { ; }
	void init() { mToggle = 0; }
	unsigned char read4015() {
		return m4015;
	}
	void enableSound(bool on) { mSoundEnable = on; }
	void reset() {
		mSound.enableChannel(0x0f);
	}
	void write4015(unsigned char val) {
		mSound.enableChannel(val&0x0f);
	}
	void writeSq0(int regNum, unsigned char val) { 
		mSq0Regs[regNum] = val;
		switch (regNum) {
		case 0: doSq0Volume(); break;
		case 1: doSq0Sweep(); break;
		case 2: doSq0Freq(); break;
		case 3: doSq0Sound(); break;
		}
	}
	void writeSq1(int regNum, unsigned char val) { 
		mSq1Regs[regNum] = val;
		switch (regNum) {
		case 0: doSq1Volume(); break;
		case 1: doSq1Sweep(); break;
		case 2: doSq1Freq(); break;
		case 3: doSq1Sound(); break;
		}
	}
	void writeTri(int regNum, unsigned char val) {
		mTriRegs[regNum] = val;
		switch (regNum) {
		case 0: break;
		case 1: break;
		case 2: doTriFreq(); break;
		case 3: doTriSound(); break;
		}
	}
	void writeNiz(int regNum, unsigned char val) {
		mNizRegs[regNum] = val;
		switch (regNum) {
		case 0: doNizVolume(); break;
		case 1: break;
		case 2: doNizFreq(); break;
		case 3: doNizSound(); break;
		}
	}
	void doSq0Sound();
	void doSq1Sound();
	void doTriSound();
	void doNizSound();
	void doSq0Volume();
	void doSq1Volume();
	void doNizVolume();
	void doSq0Freq();
	void doSq1Freq();
	void doTriFreq();
	void doNizFreq();
	void doSq0Sweep() { doSqSweep(mSq0Regs, 0); }
	void doSq1Sweep() { doSqSweep(mSq1Regs, 1); }
	void doSqSweep(unsigned char* regs, int num);
	void statusChange();
	void stop() {
		for (int i=0; i<4; i++) {
			mSound.stop(i);
		}
	}
};

#endif
