/*
        mapper #15 support

        (c)makoto 2000
*/
#ifndef Mapper015_H_DEFINED
#define Mapper015_H_DEFINED

#include "NES/Casette.h"

/* 100 in 1 */
class Mapper015 : public Casette
{
public:
  void init();
  void write(unsigned address, unsigned char data);
};

#endif
