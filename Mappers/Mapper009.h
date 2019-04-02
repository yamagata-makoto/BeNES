/*
	mapper #9 support
	
	(c)makoto 2000
*/
#ifndef Mapper009_H_DEFINED
#define Mapper009_H_DEFINED

#include "NES/Casette.h"
#include "NES/PPU.h"

/* MMC2 */
class Mapper009: public Casette,
                public PPUdelegate {
		unsigned char hi;
		unsigned char lo;
		unsigned char flag;
	struct {
		unsigned char hi;
		unsigned char lo;
		unsigned char flag;
	}data1, data2;
public:
	void init();
	void rendering(unsigned address);
	void write(unsigned address, unsigned char data);
};

#endif

