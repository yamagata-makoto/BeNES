/*
        mapper #2 support

        (c)makoto 2000
*/
#ifndef Mapper002_H_DEFINED
#define Mapper002_H_DEFINED

#include "NES/Casette.h"

/* UNROM */
class Mapper002 : public Casette
{
public:
  void write(unsigned address, unsigned char data);
  void init();
};

#endif
