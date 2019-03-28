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
#ifndef NESSOUND_H_DEFINED
#define NESSOUND_H_DEFINED

#include <stdio.h>
extern "C" {
#include "audio.h"
}
//======================================================================
struct NesSoundVoice {
	HAC mVoice;
	LPAUDIOWAVE mWave;
public:
	NesSoundVoice():
		mWave(NULL){ ; }
};

struct NesSoundSource {
	NesSoundVoice* mSound;
	NesSoundVoice* mChanged;
	unsigned short mFreq;
	long mVolume;
	long mFreqValue;
	bool mPlaying;
public:
	NesSoundSource():
		mSound(NULL), mChanged(NULL), 
		mFreq(0L), mVolume(0L), mPlaying(false) { ; }

};

//======================================================================
class NesSound {
	enum { NUMSOUNDS=10, NUMCHANNELS=4 };
	NesSoundVoice mSq0Sound[4];
	NesSoundVoice mSq1Sound[4];
	NesSoundVoice mTriSound;
	NesSoundVoice mNizSound;
	NesSoundSource mChannel[NUMCHANNELS];
	unsigned char mEnabled;
public:
	NesSound();
	~NesSound();
	void enableChannel(unsigned char enabled);
	void setFreq(int channel, unsigned short val);
	void setNoiseFreq(unsigned short val);
	void setVolume(int channel, long val);
	void stop(int channel);
	void play(int channel);
	void update(int channel);
	long getVolume(int channel) { 
		return mChannel[channel].mVolume;
	}
	unsigned short getFreq(int channel) { 
		return mChannel[channel].mFreq;
	}
	void setSq0Wave(unsigned char type);
	void setSq1Wave(unsigned char type);
};

#endif
