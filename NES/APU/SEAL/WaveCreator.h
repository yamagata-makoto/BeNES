#ifndef WAVECREATOR_H_DEFINED
#define WAVECREATOR_H_DEFINED

#include "audio.h"

class WaveCreator
{
public:
  static LPAUDIOWAVE Audio16BitMono(WORD rate, LPBYTE data, DWORD len);
};

#endif
