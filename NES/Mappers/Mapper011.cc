/*
        mapper #11 support
        thanks to grugru

        (c)makoto 2000
*/
#include "Mapper011.h"
#include "NES/PatternTable.h"
#include "NES/ROM.h"

void
Mapper011::init()
{
  PatternTable()->deepCopy8k(0x0000, chrrom.image);

  ROM()->copy16k(0x8000, prgrom.image);
  ROM()->copy16k(0xc000, &(prgrom.image[prgrom.size - 0x4000]));
}

void
Mapper011::write(unsigned address, unsigned char data)
{
  unsigned pos;
  if (prgrom.banks > 2) {
    pos = data * 0x8000;
    pos &= (prgrom.size - 1);
    ROM()->copy32k(0x8000, &(prgrom.image[pos]));
  }
  pos = (data >> 4) * 0x2000;
  pos &= (chrrom.size - 1);
  PatternTable()->copy8k(0x0000, &(chrrom.image[pos]));
}
