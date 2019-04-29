#include "WaveCreator.h"
#include <cstring>
#include <memory>

const WORD WAVE_FORMAT =
  AUDIO_FORMAT_16BITS | AUDIO_FORMAT_MONO | AUDIO_FORMAT_LOOP;

LPAUDIOWAVE
WaveCreator::Audio16BitMono(WORD rate, LPBYTE data, DWORD len)
{
  std::auto_ptr<AUDIOWAVE> wave(new AUDIOWAVE);
  wave->wFormat = WAVE_FORMAT;
  wave->nSampleRate = rate;
  wave->dwLength = len;
  wave->dwLoopStart = 0L;
  wave->dwLoopEnd = len;

  if (ACreateAudioData(wave.get()) != AUDIO_ERROR_NONE) {
    return NULL;
  }
  std::memcpy(wave->lpData, data, len);
  AWriteAudioData(wave.get(), 0L, len);

  return wave.release();
}
