/*
        mapper #1 support

        (c)makoto 2000
*/
#include "Mapper001.h"
#include "NES/PatternTable.h"
#include "NES/ROM.h"
#include "NES/VRAM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CART_OTHER (0)
#define CART_512K (1)
#define CART_1024K (2)

void
Mapper001::init()
{
  memset(regs, 0, sizeof(regs));
  counter = latch = 0;
  carttype = CART_OTHER;
  if (prgrom.size == (0x20 * 0x4000)) {
    carttype = CART_512K;
  } else if (prgrom.size == (0x40 * 0x4000)) {
    carttype = CART_1024K;
  }
  if (chrrom.size > 0) {
    prgrom_ptr = prgrom.image;
    chrrom_ptr = chrrom.image;
    prgsize = prgrom.size;
    chrsize = chrrom.size;
    ROM()->copy16k(0x8000, prgrom_ptr);
    ROM()->copy16k(0xc000, &(prgrom_ptr[prgsize - 0x4000]));
    PatternTable()->copy8k(0x0000, chrrom_ptr);
  } else {
    prgrom_ptr = prgrom.image;
    chrrom_ptr = prgrom_ptr;
    prgsize = prgrom.size;
    chrsize = prgsize;
    ROM()->copy16k(0x8000, &(prgrom_ptr[0x2000]));
    ROM()->copy16k(0xc000, &(prgrom_ptr[prgsize - 0x4000]));
    PatternTable()->deepCopy8k(0x0000, chrrom_ptr);
  }
}

void
Mapper001::write(unsigned address, unsigned char data)
{
  if (data & 0x80) {
    regs[0] = regs[1] = regs[2] = regs[3] = 0;
    regs[0] |= 0x0c;
    latch = regs[(address >> 13) & 0x03];
    counter = 5;
  } else {
    latch |= (data & 0x01) << (counter++);
  }
  if (counter == 5) {
    unsigned char num = (address >> 13) & 0x03;
    regs[num] = latch;
    counter = latch = 0;
    setMirror();
    setPRGbank();
    setCHRbank();
  }
}

int
Mapper001::calcBankbase()
{
  // calc prg bank base (buggy...)
  int bankbase = 0;
  if (carttype == CART_512K) {
    bankbase = (regs[1] & 0x10) ? 0x40000 : 0;
  } else if (carttype == CART_1024K) {
    if (regs[0] & 0x10) {
      if (regs[1] & 0x10) {
        bankbase += 0x40000;
      }
      if (regs[2] & 0x10) {
        bankbase += 0x80000;
      }
    } else {
      bankbase = (regs[1] & 0x10) ? 0xc0000 : 0;
    }
  }
  return bankbase;
}

void
Mapper001::setMirror()
{
  // mirror settings
  if (!(regs[0] & 0x02)) {
    if (regs[0] & 0x01) {
      VRAM()->setMirrorType(MIRROR_1HIGH);
    } else {
      VRAM()->setMirrorType(MIRROR_1LOW);
    }
  } else {
    if (regs[0] & 0x01) {
      VRAM()->setMirrorType(MIRROR_HORIZONTAL);
    } else {
      VRAM()->setMirrorType(MIRROR_VERTICAL);
    }
  }
}

void
Mapper001::setCHRbank()
{
  // chr bank set
  if (chrrom.size > 0) {
    if (regs[0] & 0x10) {
      PatternTable()->copy4k(0x0000, &(chrrom_ptr[regs[1] * 0x1000]));
      PatternTable()->copy4k(0x1000, &(chrrom_ptr[regs[2] * 0x1000]));
    } else {
      PatternTable()->copy8k(0x0000, &(chrrom_ptr[(regs[1] & 0xfe) * 0x1000]));
    }
  }
}

void
Mapper001::setPRGbank()
{
  int bankbase = calcBankbase();
  // prg bank set
  if (regs[0] & 0x08) {
    unsigned char* p = &(prgrom.image[(regs[3] * 0x4000) + bankbase]);
    if (regs[0] & 0x04) {
      ROM()->copy16k(0x8000, p);
      ROM()->copy16k(0xc000, &(prgrom_ptr[prgsize - 0x4000]));
    } else {
      ROM()->copy16k(0x8000, prgrom_ptr);
      ROM()->copy16k(0xc000, p);
    }
  } else {
    unsigned char* p = &(prgrom_ptr[((regs[3] & 0xfe) * 0x4000) + bankbase]);

    ROM()->copy32k(0x8000, p);
  }
}
