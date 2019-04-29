#ifndef Defs_H_DEFINED
#define Defs_H_DEFINED

typedef struct
{
  unsigned char* bits;
  unsigned pitch;
} NESscreen;

#define MIRROR_HORIZONTAL (0)
#define MIRROR_VERTICAL (1)
#define MIRROR_1HIGH (2)
#define MIRROR_1LOW (3)
#define MIRROR_4SCREEN (4)

typedef unsigned char BOOL;

#ifndef YES
#define YES (1)
#endif

#ifndef NO
#define NO (0)
#endif

#endif
