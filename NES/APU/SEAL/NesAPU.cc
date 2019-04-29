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
// Audio Processor Unit (APU)
//
#include "NesAPU.h"

const int VOL_FADE = 3;
const int VOL_MAX = 0x0f * VOL_FADE;

static const unsigned char SoundLengthTbl[0x20] = {
  0x05 * 2 - 1, 0x7f * 2 - 1, 0x0a * 2 - 1, 0x01 * 2 - 1, 0x14 * 2 - 1,
  0x02 * 2 - 1, 0x28 * 2 - 1, 0x03 * 2 - 1, 0x50 * 2 - 1, 0x04 * 2 - 1,
  0x1e * 2 - 1, 0x05 * 2 - 1, 0x07 * 2 - 1, 0x06 * 2 - 1, 0x0d * 2 - 1,
  0x07 * 2 - 1, 0x06 * 2 - 1, 0x08 * 2 - 1, 0x0c * 2 - 1, 0x09 * 2 - 1,
  0x18 * 2 - 1, 0x0a * 2 - 1, 0x30 * 2 - 1, 0x0b * 2 - 1, 0x60 * 2 - 1,
  0x0c * 2 - 1, 0x24 * 2 - 1, 0x0d * 2 - 1, 0x08 * 2 - 1, 0x0e * 2 - 1,
  0x10 * 2 - 1, 0x0f * 2 - 1
};

inline int
GET_FREQ(unsigned char* R)
{
  return (((R[3] & 0x07) << 8) | R[2]);
}
inline unsigned char
GET_WAVETYPE(unsigned char* R)
{
  return ((R[0] >> 6) & 0x03);
}
inline unsigned char
GET_SNDLENGTH(unsigned char* R)
{
  return (SoundLengthTbl[(R[3] >> 3) & 0x1f]);
}
inline int
GET_SQxLENGTH(unsigned char* R)
{
  return (R[0] & 0x20) ? -1 : GET_SNDLENGTH(R);
}
inline int
GET_VOLUME(unsigned char* R)
{
  return (R[0] & 0x10) ? ((R[0] & 0x0f) * VOL_FADE) : VOL_MAX;
}
inline int
GET_FADEVALUE(unsigned char* R)
{
  return (R[0] & 0x10) ? -1 : ((R[0] & 0x0f) + 1);
}
inline int
GET_SWEEPVALUE(unsigned char* R)
{
  return (R[1] & 0x80) ? (((R[1] & 0x70) >> 4) + 1) : -1;
}
inline unsigned char
GET_SWEEPSPEED(unsigned char* R)
{
  return (R[1] & 0x07);
}
inline unsigned char
GET_SWEEPDIRECTION(unsigned char* R)
{
  return ((R[1] >> 3) & 0x01);
}
static int NoiseFreqTable[16] = { 1,  2,  4,   8,   16,  24,  32,  40,
                                  64, 96, 128, 192, 256, 384, 512, 512 };

#define NOISE_FREQ ((NoiseFreqTable[mNizRegs[2] & 0x0f]))
#define NOISE_VOL (GET_VOLUME(mNizRegs))

void
NesAPU::doSqSweep(unsigned char* regs, int num)
{
  mSweepValue[num] = mSweepCounter[num] = GET_SWEEPVALUE(regs);
  mSweepSpeed[num] = GET_SWEEPSPEED(regs);
  mSweepDirection[num] = GET_SWEEPDIRECTION(regs);
}

void
NesAPU::doSq0Volume()
{
  int vol = GET_VOLUME(mSq0Regs);
  mSound.setSq0Wave(GET_WAVETYPE(mSq0Regs));
  mSound.setVolume(0, vol);
  mFadeValue[0] = mFadeCounter[0] = GET_FADEVALUE(mSq0Regs);
}

void
NesAPU::doSq1Volume()
{
  int vol = GET_VOLUME(mSq1Regs);
  mSound.setSq1Wave(GET_WAVETYPE(mSq1Regs));
  mSound.setVolume(1, vol);
  mFadeValue[1] = mFadeCounter[1] = GET_FADEVALUE(mSq1Regs);
}

void
NesAPU::doNizVolume()
{
  mSound.setVolume(3, NOISE_VOL);
  mFadeValue[3] = mFadeCounter[3] = GET_FADEVALUE(mNizRegs);
}

void
NesAPU::doSq0Freq()
{
  int freq = GET_FREQ(mSq0Regs);
  mSound.setFreq(0, freq);
}

void
NesAPU::doSq1Freq()
{
  int freq = GET_FREQ(mSq1Regs);
  mSound.setFreq(1, freq);
}

void
NesAPU::doTriFreq()
{
  int freq = GET_FREQ(mTriRegs);
  mSound.setFreq(2, freq);
}

void
NesAPU::doNizFreq()
{
  mSound.setNoiseFreq(NOISE_FREQ);
  mSound.setVolume(3, NOISE_VOL);
}

void
NesAPU::doSq0Sound()
{
  int vol = GET_VOLUME(mSq0Regs);
  int freq = GET_FREQ(mSq0Regs);
  mLength[0] = GET_SQxLENGTH(mSq0Regs);
  mSound.setSq0Wave(GET_WAVETYPE(mSq0Regs));
  mSound.setFreq(0, freq);
  mSound.setVolume(0, vol);
  mSound.stop(0);
  // if (mSoundEnable)
  mSound.play(0);
  m4015 |= 0x01;
  mFadeValue[0] = mFadeCounter[0] = GET_FADEVALUE(mSq0Regs);
}

void
NesAPU::doSq1Sound()
{
  int vol = GET_VOLUME(mSq1Regs);
  int freq = GET_FREQ(mSq1Regs);
  mLength[1] = GET_SQxLENGTH(mSq1Regs);
  mSound.setSq1Wave(GET_WAVETYPE(mSq1Regs));
  mSound.setFreq(1, freq);
  mSound.setVolume(1, vol);
  mSound.stop(1);
  // if (mSoundEnable)
  mSound.play(1);
  m4015 |= 0x02;
  mFadeValue[1] = mFadeCounter[1] = GET_FADEVALUE(mSq1Regs);
}

void
NesAPU::doTriSound()
{
  int freq = GET_FREQ(mTriRegs);
  if ((mTriRegs[0] & 0x80) /*|| (!mTriRegs[0])*/) {
    mLength[2] = mTriLen = -1;

  } else {
    if (mTriRegs[0] & 0x7f) {
      mLength[2] = GET_SNDLENGTH(mTriRegs);
      mTriLen = (mTriRegs[0] & 0x7f);
      mSound.setVolume(2, VOL_MAX);
    } else {
      mLength[2] = mTriLen = -1;
      mSound.setVolume(2, 0);
    }
  }
  mSound.setFreq(2, freq);
  // if (mSoundEnable)
  mSound.play(2);
  m4015 |= 0x04;
  mFadeValue[2] = mFadeCounter[2] = -1;
}

void
NesAPU::doNizSound()
{
  mLength[3] = GET_SQxLENGTH(mNizRegs);
  mSound.setVolume(3, NOISE_VOL);
  mSound.setNoiseFreq(NOISE_FREQ);
  // if (mSoundEnable)
  mSound.play(3);
  m4015 |= 0x08;
  mFadeValue[3] = mFadeCounter[3] = GET_FADEVALUE(mNizRegs);
}

void
NesAPU::statusChange()
{
  for (int i = 0; i < 4; i++) {
    if (m4015 & (0x01 << i)) {
      // if (mSoundEnable)
      mSound.update(i);
    }
    // UPDATE SOUND VOLUME
    if ((mFadeCounter[i] > 0) && (--mFadeCounter[i] == 0)) {
      mFadeCounter[i] = mFadeValue[i];
      long vol = mSound.getVolume(i) - VOL_FADE;
      if (vol > 0) {
        mSound.setVolume(i, vol);
      } else if (mLength[i] == -1) {
        mSound.setVolume(i, VOL_MAX);
      } else {
        mSound.setVolume(i, 0);
        m4015 &= ~(0x01 << i);
      }
    } else if (mFadeCounter[i] == 0) {
      mSound.setVolume(i, 0);
      m4015 &= ~(0x01 << i);
    }
    if ((mToggle == 1) || (mToggle == 3)) {
      // UPDATE TRI LENGTH
      if ((mToggle == 1) && (mTriLen > 0) && (--mTriLen == 0)) {
        m4015 &= ~(0x01 << 2);
        mSound.stop(2);
      }
#if 0
			// UPDATE SOUND VOLUME
			if ((mFadeCounter[i]>0) && (--mFadeCounter[i]==0)) {
				mFadeCounter[i] = mFadeValue[i];
				long vol = mSound.getVolume(i) - VOL_FADE;
				if (vol > 0) {
					mSound.setVolume(i, vol);
				}else if (mLength[i] == -1) {
					mSound.setVolume(i, VOL_MAX);
				}else {
					mSound.setVolume(i, 0);
				}
			}else if (mFadeCounter[i] == 0) {
				mSound.setVolume(i, 0);
			}
#endif
      // SQ SWEEP
      if ((mToggle == 3) && (i <= 1)) {
        if ((mSweepCounter[i] > 0) && (--mSweepCounter[i] == 0)) {
          mSweepCounter[i] = mSweepValue[i];
          int freq = mSound.getFreq(i);
          if (mSweepDirection[i]) {
            if (mSweepSpeed[i]) {
              if (i == 0)
                freq += ~(freq >> mSweepSpeed[0]);
              else
                freq -= (freq >> mSweepSpeed[1]);
            }
          } else {
            if (mSweepSpeed[i])
              freq += (freq >> mSweepSpeed[i]);
          }
          if ((freq > 0x08) && (freq < 0x3ff)) {
            mSound.setFreq(i, freq);
          } else {
            mSound.stop(i);
            m4015 &= ~(0x01 << i);
          }
        }
      }
      // UPDATE SOUND LENGTH
      if ((mToggle == 3) && (mLength[i] > 0) && (--mLength[i] == 0)) {
        mSound.stop(i);
        m4015 &= ~(0x01 << i);
      }
    }
  }
  mToggle = (mToggle + 1) & 0x03;
}
