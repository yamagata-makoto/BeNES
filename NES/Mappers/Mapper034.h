/*
        mapper #34 support

        (c)makoto 2000
*/
#ifndef Mapper034_H_DEFINED
#define Mapper034_H_DEFINED

#include "NES/Casette.h"

/* iNES Mapper #34 */
class Mapper034 : public Casette
{
public:
  void write(unsigned address, unsigned char data);
  void init();
};

#endif
