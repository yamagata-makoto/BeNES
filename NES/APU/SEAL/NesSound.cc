#include "NesSound.h"
#include "WaveCreator.h"

unsigned short SQ0_882Hz[50] = {
  0x3636, 0x574B, 0x4EB4, 0x52D4, 0x4E11, 0x5666, 0x21EB, 0xEC27, 0xF33E,
  0xF0A4, 0xF12A, 0xF194, 0xF0DD, 0xF13D, 0xF12A, 0xF121, 0xF183, 0xF14A,
  0xF174, 0xF1DE, 0xF17C, 0xF1B9, 0xF203, 0xF19E, 0xF208, 0xF21A, 0xF1D9,
  0xF25A, 0xF231, 0xF214, 0xF28C, 0xF24F, 0xF253, 0xF294, 0xF261, 0xF263,
  0xF281, 0xF27B, 0xF29B, 0xF2B2, 0xF2A3, 0xF2C6, 0xF2CD, 0xF2CC, 0xF2F2,
  0xF3BF, 0xF2AB, 0xF3FF, 0xF3BF, 0xF10B
};

unsigned short SQ1_882Hz[50] = {
  0x3984, 0x47B3, 0x41F5, 0x450C, 0x42CA, 0x436F, 0x4325, 0x4207, 0x420B,
  0x4217, 0x3F16, 0x46F3, 0x1357, 0xDE89, 0xE582, 0xE30B, 0xE39C, 0xE420,
  0xE36F, 0xE3CB, 0xE3C5, 0xE3EA, 0xE42B, 0xE42D, 0xE44D, 0xE49B, 0xE484,
  0xE4C5, 0xE4EC, 0xE4E2, 0xE4E6, 0xE510, 0xE516, 0xE555, 0xE561, 0xE574,
  0xE5A2, 0xE5BB, 0xE5CA, 0xE60F, 0xE607, 0xE61F, 0xE650, 0xE64F, 0xE68D,
  0xE79B, 0xE5BA, 0xE93F, 0xE44F, 0xEED7
};

unsigned short SQ2_882Hz[50] = {
  0x2DCF, 0x30C1, 0x2E8E, 0x302F, 0x2EB1, 0x2F3A, 0x2EF9, 0x2EDD, 0x2E91,
  0x2E33, 0x2E06, 0x2DAF, 0x2D89, 0x2D50, 0x2D05, 0x2CD5, 0x2CA4, 0x2C50,
  0x2C46, 0x2BC5, 0x2ACC, 0x2C98, 0x2809, 0x3050, 0x160B, 0xD1FA, 0xCD1D,
  0xD017, 0xCDE0, 0xCFBC, 0xCF1C, 0xCF30, 0xCF7A, 0xCFBA, 0xCFE8, 0xD04A,
  0xD06C, 0xD0A7, 0xD0FD, 0xD11B, 0xD163, 0xD1AD, 0xD1C7, 0xD207, 0xD26A,
  0xD34B, 0xD1B4, 0xD610, 0xCE0A, 0xE64B
};

unsigned short SQ3_882Hz[50] = {
  0x17E8, 0x1819, 0x16EF, 0x1866, 0x1715, 0x17C6, 0x17B1, 0x179F, 0x175B,
  0x175C, 0x172A, 0x170F, 0x1700, 0x16CE, 0x16A5, 0x16A1, 0x1666, 0x166D,
  0x1656, 0x1619, 0x15F3, 0x15E2, 0x15B9, 0x15C4, 0x159D, 0x1578, 0x1570,
  0x154A, 0x1532, 0x152F, 0x14EA, 0x14EE, 0x1405, 0x14FA, 0x13A8, 0x1423,
  0x15BA, 0xD77C, 0xB373, 0xBC2E, 0xB8A9, 0xBA5A, 0xBA6D, 0xBA82, 0xBAED,
  0xBC01, 0xBA9D, 0xBF03, 0xB6E5, 0xD378
};

unsigned short TRI_882Hz[50] = {
  0x01D3, 0x07D9, 0x0E0A, 0x140C, 0x19BA, 0x1FA3, 0x2527, 0x2ACC, 0x309C,
  0x35AE, 0x3AB6, 0x4041, 0x44BF, 0x45B5, 0x4423, 0x3E03, 0x387D, 0x32C2,
  0x2CC8, 0x271B, 0x20D8, 0x1AC5, 0x1511, 0x0EB8, 0x0887, 0x0291, 0xFC15,
  0xF5CC, 0xEFA2, 0xE909, 0xE2C7, 0xDC83, 0xD575, 0xCF01, 0xC88E, 0xC1B5,
  0xBB7D, 0xB41F, 0xB15F, 0xB326, 0xB9A8, 0xC159, 0xC898, 0xCF73, 0xD69E,
  0xDE00, 0xE496, 0xEB55, 0xF207, 0xF845
};

const double SOUND_CONST = 111860.78 / 882 * 44100;
const double NOISE_CONST = (44100.0 * 13);
const int M_BITS = 13;
const int NOISE_BUFSIZE = (1 << M_BITS);

unsigned short NOISE[NOISE_BUFSIZE];

const int WAVE_LENGTH = 100;
const WORD AUDIO_FORMAT =
  AUDIO_FORMAT_16BITS | AUDIO_FORMAT_MONO | AUDIO_FORMAT_LOOP;

//======================================================================
inline void
createSound(NesSoundVoice& sound, unsigned short* data, int len)
{
  sound.mWave =
    WaveCreator::Audio16BitMono(44100, reinterpret_cast<LPBYTE>(data), len);
  sound.mWave->wFormat |= AUDIO_FORMAT;
  sound.mWave->dwLoopStart = 0L;
  sound.mWave->dwLoopEnd = sound.mWave->dwLength;
  ACreateAudioVoice(&(sound.mVoice));
  ASetVoicePanning(sound.mVoice, 128);
}

inline void
destroySound(NesSoundVoice& sound)
{
  AStopVoice(sound.mVoice);
  ADestroyAudioVoice(sound.mVoice);
  AFreeWaveFile(sound.mWave);
}

inline void
createNoise(unsigned short* buf, const int bits, int size)
{
  static int m = 0x0011;
  int i, xor_;

  for (i = 0; i<size>> 1; i++) {
    xor_ = m & 1;
    m >>= 1;
    xor_ ^= m & 1;
    m |= xor_ << (bits - 1);
    if (m & (1 << (bits - 1))) /* if MSB = 1 */
      buf[i] = (short)(-m);
    else
      buf[i] = (short)m;
  }
}

//----------------------------------------------------------------------
NesSound::NesSound()
  : mEnabled(0)
{
  static unsigned short* sq_waves[4] = {
    SQ0_882Hz,
    SQ1_882Hz,
    SQ2_882Hz,
    SQ3_882Hz,
  };
  // initialize & open audio
  AUDIOINFO info;
  AInitialize();
  info.nDeviceId = AUDIO_DEVICE_MAPPER;
  info.wFormat = AUDIO_FORMAT_16BITS | AUDIO_FORMAT_MONO;
  info.nSampleRate = 44100;
  AOpenAudio(&info);
  // regist voices
  AOpenVoices(NUMSOUNDS);
  // Create SQ Sound
  for (int i = 0; i < 4; i++) {
    createSound(mSq0Sound[i], sq_waves[i], WAVE_LENGTH);
    createSound(mSq1Sound[i], sq_waves[i], WAVE_LENGTH);
  }
  // Create Tri Sound
  createSound(mTriSound, TRI_882Hz, WAVE_LENGTH);
  // Create Niz Sound
  createNoise(NOISE, M_BITS, sizeof(NOISE));
  createSound(mNizSound, NOISE, sizeof(NOISE));

  mChannel[0].mSound = &mSq0Sound[0];
  mChannel[1].mSound = &mSq1Sound[0];
  mChannel[2].mSound = &mTriSound;
  mChannel[3].mSound = &mNizSound;
}

//----------------------------------------------------------------------
NesSound::~NesSound()
{
  // Free SQ Sound
  for (int i = 0; i < 4; i++) {
    destroySound(mSq0Sound[i]);
    destroySound(mSq1Sound[i]);
  }
  // Free Tri Sound
  destroySound(mTriSound);
  // Free Niz Sound
  destroySound(mNizSound);

  ACloseVoices();
  // close audio
  ACloseAudio();
}

//----------------------------------------------------------------------
void
NesSound::setVolume(int channel, long val)
{
  mChannel[channel].mVolume = val;
}

//----------------------------------------------------------------------
void
NesSound::setFreq(int channel, unsigned short val)
{
  long freq = static_cast<long>(SOUND_CONST / (val + 1));
  mChannel[channel].mFreqValue = freq;
  mChannel[channel].mFreq = val;
}

//----------------------------------------------------------------------
void
NesSound::setNoiseFreq(unsigned short val)
{
  long freq = static_cast<long>((NOISE_CONST / val));
  mChannel[3].mFreqValue = freq;
  mChannel[3].mFreq = val;
}

//----------------------------------------------------------------------
void
NesSound::play(int channel)
{
  NesSoundSource* current = &mChannel[channel];
  if (mEnabled & (0x01 << channel)) {
    if (channel == 3) {
      ASetVoicePosition(current->mSound->mVoice, 0);
    }
    APrimeVoice(current->mSound->mVoice, current->mSound->mWave);
    ASetVoiceFrequency(current->mSound->mVoice, current->mFreqValue);
    ASetVoiceVolume(current->mSound->mVoice, current->mVolume);
    AStartVoice(current->mSound->mVoice);
    current->mPlaying = true;
  }
}

//----------------------------------------------------------------------
void
NesSound::update(int channel)
{
  NesSoundSource* current = &mChannel[channel];
  if (mEnabled & (0x01 << channel)) {
    if (channel == 3) {
      ASetVoicePosition(current->mSound->mVoice, 0);
    }
    if (current->mChanged) {
      AStopVoice(current->mSound->mVoice);
      current->mSound = current->mChanged;
      current->mChanged = NULL;
      APrimeVoice(current->mSound->mVoice, current->mSound->mWave);
      ASetVoiceFrequency(current->mSound->mVoice, current->mFreqValue);
      ASetVoiceVolume(current->mSound->mVoice, current->mVolume);
      AStartVoice(current->mSound->mVoice);
    } else {
      ASetVoiceFrequency(current->mSound->mVoice, current->mFreqValue);
      ASetVoiceVolume(current->mSound->mVoice, current->mVolume);
    }
  }
}

//----------------------------------------------------------------------
void
NesSound::stop(int channel)
{
  NesSoundSource* current = &mChannel[channel];
  current->mPlaying = false;
  ASetVoiceVolume(current->mSound->mVoice, 0);
  AStopVoice(current->mSound->mVoice);
}

//----------------------------------------------------------------------
void
NesSound::enableChannel(unsigned char enabled)
{
  mEnabled = enabled;
  for (int i = 0; i < NUMCHANNELS; i++) {
    if (!(mEnabled & (0x01 << i))) {
      stop(i);
    }
  }
}

//----------------------------------------------------------------------
void
NesSound::setSq0Wave(unsigned char type)
{
  NesSoundSource* current = &mChannel[0];
  if (current->mSound != &mSq0Sound[type & 0x03]) {
    current->mChanged = &mSq0Sound[type & 0x03];
  }
}

//----------------------------------------------------------------------
void
NesSound::setSq1Wave(unsigned char type)
{
  NesSoundSource* current = &mChannel[1];
  if (current->mSound != &mSq1Sound[type & 0x03]) {
    current->mChanged = &mSq1Sound[type & 0x03];
  }
}

//======================================================================
//#define TEST
#ifdef TEST
int
main()
{
  NesSound sound;
  unsigned char a4015;
  unsigned char a4000 = 0x3f;
  unsigned char a4001 = 0x00;
  unsigned char a4002 = 0x7e;
  unsigned char a4003 = 0x00;

  // 4015 write
  a4015 = 0x01;
  sound.enableChannel(a4015);

  // 4003 write
  sound.setSquareWaveType(0, (a4000 >> 6) & 0x03);
  unsigned short val = ((a4003 & 0x07) << 8) | a4002;
  val = static_cast<short>(111860.78 / (val + 1));
  sound.play(0, 441);

  getchar();

  // 4015 write
  a4015 = 0;
  sound.enableChannel(a4015);

  return 0;
}
#endif
