/*
        mapper #11 support

        (c)makoto 2000
*/
#ifndef Mapper011_H_DEFINED
#define Mapper011_H_DEFINED

#include "NES/Casette.h"

/* Color Dreams */
class Mapper011 : public Casette
{
public:
  void write(unsigned address, unsigned char data);
  void init();
};

#endif
