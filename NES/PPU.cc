#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "PPU.h"
#include "VRAM.h"
#include "PatternTable.h"
#include "PaletteIndex.h"


#define DRAW_BEG_LINE (8)
#define DRAW_END_LINE (231)

static PPUdelegate* delegate = NULL;

PPUprotocol PPUprotocol::instance;

/* PPU registers */
static unsigned char reg2000; /* control reg.#1 */ 
static unsigned char reg2001; /* control reg.#2 */
static unsigned char reg2002; /* status reg. */ 
static unsigned char reg2003; /* sprite RAM address */ 
static unsigned char reg2004; /* sprite RAM I/O */  
static unsigned char reg2005; /* scroll reg. */ 
static unsigned char reg2006; /* VRAM address */ 
static unsigned char reg2007; /* VRAM I/O */ 

/* VRAM address reg. ($2000/$2005/$2006) */
static unsigned vramAddress;

/* VRAM address temp reg. ($2000/$2005/$2006) */
static unsigned short tempAddress;

/* horizontal scroll value */
static unsigned char valXscroll;

/* latch register */
static unsigned char regLatch;

/* 2005/2006 write flag */
static BOOL firstWrite;

/* RAM */
static unsigned char paletteRAM[32];
static unsigned char spriteRAM[256];

/* from PatternTable interface */
extern unsigned char* patternTable[8];
extern PatternTile cachedTileA[0x100];
extern PatternTile cachedTileB[0x100];

/* from VRAM interface */
extern unsigned char* nameTable[4];
extern unsigned char* cachedAttrTable[4];

/* access macros */
#define PATTERN_TABLE(address) \
	(patternTable[((address)>>10)&0x07][(address)&0x3ff])

#define NAME_TABLE(address) \
	(nameTable[((address)>>10)&0x03][(address)&0x3ff])

#define CACHED_ATTR_TABLE(address) \
	(cachedAttrTable[((address)>>10)&0x03][(address)&0x3ff])

/* register i/o functions */
static unsigned char read2000();
static unsigned char read2001();
static unsigned char read2002();
static unsigned char read2003();
static unsigned char read2004();
static unsigned char read2005();
static unsigned char read2006();
static unsigned char read2007();

static void write2000(unsigned char data);
static void write2001(unsigned char data);
static void write2002(unsigned char data);
static void write2003(unsigned char data);
static void write2004(unsigned char data);
static void write2005(unsigned char data);
static void write2006(unsigned char data);
static void write2007(unsigned char data);

unsigned char PPUread(unsigned address);
void PPUwrite(unsigned address, unsigned char data);

/* register i/o table */
typedef unsigned char (*ReadFuncPtr)();
typedef void (*WriteFuncPtr)(unsigned char);

static ReadFuncPtr readFuncs[8] = {
	read2000, read2001, read2002, read2003,
	read2004, read2005, read2006, read2007,
};

static WriteFuncPtr writeFuncs[8] = {
	write2000, write2001, write2002, write2003,
	write2004, write2005, write2006, write2007,
};

/* sprite data structure */
typedef struct {
	unsigned char y;
	unsigned char tile_index;
	unsigned char attribute;
	unsigned char x;
} SpriteData;

/* register access macros */
#define EXECUTE_NMI_ON_VBLANK()    ((reg2000&0x80)? YES: NO)
#define GET_SPRITE_HEIGHT()        ((reg2000&0x20)? 16: 8)
#define IS_SPRITE_VISIBLE()        ((reg2001&0x10)? YES: NO)
#define IS_SCREEN_VISIBLE()        ((reg2001&0x08)? YES: NO)
#define SET_VBLANK()               (reg2002|=0x80)
#define SET_SPRITEHIT()            (reg2002|=0x40) 
#define RESET_VBLANK()             (reg2002&=~(0x80))
#define RESET_SPRITEHIT()          (reg2002&=~(0x40)) 
#define IS_VBLANK()                ((reg2002&0x80)? YES: NO)
#define SCREENPATTERN_TABLE_ADDRESS() (((unsigned)(reg2000&0x10))<<8)
#define SPRITEPATTERN_TABLE_ADDRESS() (((unsigned)(reg2000&0x08))<<9)

/* private functions */
void renderScanLine(NESscreen* screen, unsigned scanline);
void renderScreen(char* p, char* end, unsigned scanline);
void renderSprite(char* p, char* end, unsigned scanline);
void getScreenTile(PatternTile* tile, unsigned address, unsigned tileY);
void getSpriteTile(PatternTile* tile, SpriteData* data, unsigned tileY);
void makeCachedAttrTable(unsigned address, unsigned char data);

unsigned char line[256];

/*------------------------------------ PPU Interface definition ---*/
void PPUprotocol::reset()
{
	VRAM()->reset();
	PatternTable()->reset();

	regLatch = 0x00;
	memset(spriteRAM, 0, sizeof(spriteRAM));
	
	reg2000 = 0;
	reg2001 = 0;
	reg2002 = 0;
	reg2003 = 0;
	reg2004 = 0;
	reg2005 = 0;
	reg2006 = 0;
	reg2007 = 0;
	
	vramAddress = tempAddress = 0;
	valXscroll = 0;
	
	firstWrite = YES;

	RESET_VBLANK();
	RESET_SPRITEHIT();
}

void PPUprotocol::setVBLANK()
{
	SET_VBLANK();	
	RESET_SPRITEHIT();
}

void PPUprotocol::resetVBLANK()
{
	RESET_VBLANK();
}

void PPUprotocol::spriteDMA(unsigned char* data)
{
	memcpy(spriteRAM, data, 0x100);
}

BOOL PPUprotocol::isExecuteNMIonVBLANK()
{
	return EXECUTE_NMI_ON_VBLANK();	
}

BOOL PPUprotocol::isScreenVisible()
{
	return IS_SCREEN_VISIBLE();
}

void PPUprotocol::drawScanline(unsigned scanline, NESscreen* screen)
{
	if ((IS_SCREEN_VISIBLE()==YES) || (IS_SPRITE_VISIBLE()==YES)) {
		/* frame start 
		   v=t */
		if (scanline == 0) {
			vramAddress = tempAddress;
		}
		/* scanline start 
		   v:0000010000011111=t:0000010000011111 */
		vramAddress &= ~(0x41f);
		vramAddress |= (tempAddress & 0x41f);
		renderScanLine(screen, scanline);
	}
}

void PPUprotocol::setDelegate(PPUdelegate* aDelegate)
{
	delegate = aDelegate;	
}

/* PPU I/O */
unsigned char PPUread(unsigned address)
{
	return (*readFuncs[address&0x07])();
}

void PPUwrite(unsigned address, unsigned char data)
{
	(*writeFuncs[address&0x07])(data);
}

PPUprotocol* PPU()
{
	return PPUprotocol::Instance();
}

/*-------------------------------------------- private functions ---*/
void getScreenTile(PatternTile* tile, unsigned address, unsigned tileY)
{
	unsigned ntAddress = 0x2000|(address&0x0fff);
	unsigned ptAddress = SCREENPATTERN_TABLE_ADDRESS() |
	                       (NAME_TABLE(ntAddress)*16) + tileY;
	if (delegate) {
		delegate->rendering(ptAddress);
	}
	unsigned char tileA = PATTERN_TABLE(ptAddress);
	unsigned char tileB = PATTERN_TABLE(ptAddress+8);
	unsigned char hi2bit = CACHED_ATTR_TABLE(ntAddress);
	unsigned char colorH, colorL;
	int i;
	for (i=0; i<8; i++) {
		colorL = cachedTileA[tileA].data[i];	
		colorH = cachedTileB[tileB].data[i];
		unsigned char color = hi2bit|colorH|colorL;
		tile->data[i] = color;
	}
}

void getSpriteTile(PatternTile* tile, SpriteData* data, unsigned tileY)
{
	unsigned char hi2bit = (data->attribute & 0x03)<<2;
	unsigned height = GET_SPRITE_HEIGHT();
	unsigned index = data->tile_index;
	unsigned ptAddress = SPRITEPATTERN_TABLE_ADDRESS();
	unsigned char tileA, tileB;
	unsigned char colorH, colorL;
	int i, pos;

	/* vertical flip */
	if (data->attribute & 0x80) tileY = (height-1) - tileY;
	if (height == 16) {
		if (index & 0x01) { index >>= 1; index |= 0x80; }
		else              { index >>= 1; }
		ptAddress = 0x0000;
	}
	ptAddress |= (index*(height*2)) + (tileY&0x07) + ((tileY&0x08)<<1);
	if (delegate) {
		delegate->rendering(ptAddress);
	}

	tileA =  PATTERN_TABLE(ptAddress);
	tileB =  PATTERN_TABLE(ptAddress+8);
	if ((tileA==0) && (tileB==0)) {
		memset(tile->data, 0xff, 8);
	}
	else {
		if ((void*)data == (void*)spriteRAM) {
			SET_SPRITEHIT();
		}
		for (i=0; i<8; i++) {
			/* horizontal flip */
			if (data->attribute & 0x40)  pos = 7 - i; 
			else                         pos = i;

			/* make tile data */
			colorL = cachedTileA[tileA].data[i];	
			colorH = cachedTileB[tileB].data[i];	
			unsigned char color = hi2bit|colorH|colorL;
			if ((colorH|colorL) != 0) {
				tile->data[pos] = color;
			}
			else {
				tile->data[pos] = 0xff; // transparent
			}
		}
	}
}

void renderScreen(char* p, char* end, unsigned scanline)
{
	PatternTile tile;
	static int  tileY = 0;
	int         tileX = 0;
	unsigned    firstX = valXscroll;
	int x = 0;

	tileY = ((vramAddress&0x7000)>>12) & 0x07;
		
	if ((scanline>=DRAW_BEG_LINE) && (scanline<=DRAW_END_LINE)) {
		while (p < end) {
			getScreenTile(&tile, vramAddress, tileY);
			for (tileX=firstX; tileX<8; tileX++) {
				if ((x>=8) && (x<248)) {
					*p = paletteRAM[tile.data[tileX]];
					*(p+1) = *p;
					line[x] = tile.data[tileX];
				}
				p+=2;
				x++;
				if (p >= end) break;
			}
			firstX = 0;
			if ((vramAddress&0x1f) == 0x1f) {
				vramAddress &= ~(0x1f);
				vramAddress ^= 0x0400;
			}
			else {
				vramAddress++;
			}
		}
	}
	tileY = (tileY+1) & 0x07;
	vramAddress &= ~0x7000;
	vramAddress |= ((tileY&0x07)<<12);
	
	if (tileY == 0) {
/*		vramAddress += 32;
		if ((vramAddress&0x3ff) > 0x3a0) {
			vramAddress &= ~(0x3e0);
			vramAddress ^= 0x800;
		}
*/
		if ((vramAddress&0x3e0) == 0x3a0) {
			vramAddress &= ~(0x3e0);
			vramAddress ^= 0x800;
		}
		else if ((vramAddress&0x3e0) == 0x3e0) {
			vramAddress &= ~(0x3e0);
		}
		else {
			vramAddress += 32;
		}
	}
}

void renderSprite(char* beg, char* end, unsigned scanline)
{
	PatternTile tile;
	SpriteData* spriteData = (SpriteData*)spriteRAM;
	unsigned height = GET_SPRITE_HEIGHT();
	unsigned tileY, tileX;
	unsigned x;
	
	if ((DRAW_BEG_LINE>scanline) || (DRAW_END_LINE<scanline))
		return;
		
	for (int i=63; i>=0; i--) {
		unsigned y = spriteData[i].y + 1;
		if ((scanline<y) || (scanline>=(y+height))) continue; 
		tileY = scanline - y;
		getSpriteTile(&tile, &spriteData[i], tileY); 
		for (tileX=0; tileX<8; tileX++) {
			x = spriteData[i].x + tileX;
			char* p = (beg+(x<<1));
			if (p >= end) break;
			if (tile.data[tileX] != 0xff) {
				if ((i==0) && ((line[x]&0x03)!=0)) {
					SET_SPRITEHIT();
				}
				if ((spriteData[i].attribute&0x20) && ((line[x]&0x03)!=0)) {
					if ((x>=8) && (x<248)) {
						*p = paletteRAM[(line[x]&0x0f)];
						*(p+1) = *p;
						line[x] |= 0x80;
					}
				}
				if (!(spriteData[i].attribute&0x20) || ((line[x]&0x03)==0)) {
					if ((line[x]&0x03)==0) line[x] &= ~0x80;
					if ((x>=8) && (x<248) && !(line[x]&0x80)) {
						*p = paletteRAM[0x10|tile.data[tileX]];
						*(p+1) = *p;
					}
				}
			}
		}
	}
}

/*-------------------------------------------- render scan line ---*/
void renderScanLine(NESscreen* screen, unsigned scanline)
{
	char* beg = ((char*)screen->bits)+(screen->pitch*(scanline<<1));
	char* end = ((char*)(beg)) + 512;

	if (IS_SCREEN_VISIBLE() == YES) {
		renderScreen(beg, end, scanline);
	}
	else if ((scanline>=DRAW_BEG_LINE) && (scanline<=DRAW_END_LINE)) {
		memset(beg+16, paletteRAM[0], ((end-(beg+16))+1)-16);
	}
	if (IS_SPRITE_VISIBLE() == YES) {
		renderSprite(beg, end, scanline);
	}
}

/*-------------------------------------- PPU register I/O Funcs ---*/
unsigned char read2000()
{
	/* printf("invalid read 2000\n"); */
	return regLatch;
}

void write2000(unsigned char data)
{
	reg2000 = data;
	regLatch = data;

	/* t = another temp vram address
	   2000 write:
	        t:0000110000000000=d:00000011
	*/
	tempAddress &= ~(0x0c00);
	tempAddress |= (((unsigned short)(reg2000)<<10) & 0x0c00);
}

unsigned char read2001()
{
	/* printf("invalid read 2001\n"); */
	return regLatch;
}

void write2001(unsigned char data)
{
	reg2001 = data;
	regLatch = data;
}

unsigned char read2002()
{
	regLatch &= ~(0xe0);
	regLatch |= reg2002 & 0xe0;

	/* clear VBlank bit */
	RESET_VBLANK();

	/* reset write flags(2005/2006) */
	firstWrite = YES;

	return regLatch;
}

void write2002(unsigned char data)
{
	/* printf("invalid write 2002\n"); */
	regLatch = data;
}

unsigned char read2003()
{
	/* printf("invalid read 2003\n"); */
	return regLatch;
}

void write2003(unsigned char data)
{
	reg2003 = data;
	regLatch = data;
}

unsigned char read2004()
{
	/* printf("invalid read 2004\n"); */
	return regLatch;
}

void write2004(unsigned char data)
{
	reg2004 = data;
	regLatch = data;

	spriteRAM[reg2003] = reg2004;
	reg2003 = (++reg2003) & 0xff;
}

unsigned char read2005()
{
	/* printf("invalid read 2005\n"); */
	return regLatch;
}

void write2005(unsigned char data)
{
	reg2005 = data;
	regLatch = data;

	if (firstWrite == YES) {
		firstWrite = NO;
		/*
		  t: temp vram address
		  x: tile x offset 
          2005 first write:
               t:0000000000011111=d:11111000
               x=d:00000111
		*/
		tempAddress &= ~(0x001f);
		tempAddress |= ((reg2005>>3) & 0x1f);

		valXscroll  = (reg2005 & 0x07);
	}
	else {
		firstWrite = YES;
		/*
		  t: temp vram address
		  x: offset 
          2005 second write:
		       t:0000001111100000=d:11111000
		       t:0111000000000000=d:00000111
		*/
		tempAddress &= ~(0x03e0);
		tempAddress |= (((unsigned)(reg2005))<<2) & 0x03e0;
		tempAddress &= ~(0x7000);
		tempAddress |= (((unsigned)(reg2005))<<12) & 0x7000;
	}
}

unsigned char read2006()
{
	/* printf("invalid read 2006\n"); */
	return regLatch;
}

void write2006(unsigned char data)
{
	reg2006 = data;
	regLatch = data;

	if (firstWrite == YES) {
		firstWrite = NO;
		/*
		   2006 first write:
				t:0011111100000000=d:00111111
		        t:1100000000000000=0
		*/
		tempAddress &= ~(0xff00);
		tempAddress |= (((unsigned)(reg2006))<<8) & 0xff00;
	}
	else {
		firstWrite = YES;
		/*
		   2006 second write:
		        t:0000000011111111=d:11111111
		        v=t
		*/
		tempAddress &= ~(0x00ff);
		tempAddress |= reg2006;
		vramAddress = tempAddress;
	}
}

unsigned char read2007()
{
	unsigned address = vramAddress;
	regLatch = reg2007;
	if (address < 0x2000) {
		reg2007 = PATTERN_TABLE(address);
	}
	else if (address < 0x3f00) {
		reg2007 = NAME_TABLE(address);
	}
	else {
		reg2007 = 0x20;
	}
	vramAddress += (reg2000&0x04)? 32: 1;
	return regLatch;
}

void write2007(unsigned char data)
{
	extern BOOL PatternTableWriteProtect;
	unsigned address = vramAddress;

	regLatch = data;
	reg2007 = data;

	if (address < 0x2000) {
		if (PatternTableWriteProtect == NO) {
			PATTERN_TABLE(address) = data;
		}
	}
	else if (address < 0x3f00) {
		NAME_TABLE(address) = data;
		if ((address&0x03c0) == 0x03c0) {
			makeCachedAttrTable(address, data);
		}
	}
	else {
		if ((address==0x3f00) || (address==0x3f10)) {
			paletteRAM[0x00] = paletteRAM[0x04] = 
			paletteRAM[0x08] = paletteRAM[0x0c] = 
			paletteRAM[0x10] = paletteRAM[0x14] = 
			paletteRAM[0x18] = paletteRAM[0x1c] = 
				PaletteIndex()->toIndex(data&0x3f);
		}
		else if (address & 0x03) {
			paletteRAM[address&0x001f] = 
				PaletteIndex()->toIndex(data&0x3f);
		}
	}
	vramAddress += (reg2000&0x04)? 32: 1;
}

void makeCachedAttrTable(unsigned address, unsigned char data)
{
	static const unsigned addressTable[0x40] = {
    	0x000, 0x004, 0x008, 0x00c, 0x010, 0x014, 0x018, 0x01c,
		0x080, 0x084, 0x088, 0x08c, 0x090, 0x094, 0x098, 0x09c,
		0x100, 0x104, 0x108, 0x10c, 0x110, 0x114, 0x118, 0x11c,
		0x180, 0x184, 0x188, 0x18c, 0x190, 0x194, 0x198, 0x19c,
		0x200, 0x204, 0x208, 0x20c, 0x210, 0x214, 0x218, 0x21c,
		0x280, 0x284, 0x288, 0x28c, 0x290, 0x294, 0x298, 0x29c,
		0x300, 0x304, 0x308, 0x30c, 0x310, 0x314, 0x318, 0x31c,
		0x380, 0x384, 0x388, 0x38c, 0x390, 0x394, 0x398, 0x39c
	};
	unsigned pos = (address&0xfc00)|addressTable[address&0x003f]; 

	CACHED_ATTR_TABLE(pos+0x00) = 
	CACHED_ATTR_TABLE(pos+0x01) =
	CACHED_ATTR_TABLE(pos+0x20) = 
	CACHED_ATTR_TABLE(pos+0x21) = (data&0x03) << 2;

	CACHED_ATTR_TABLE(pos+0x02) = 
	CACHED_ATTR_TABLE(pos+0x03) =
	CACHED_ATTR_TABLE(pos+0x22) = 
	CACHED_ATTR_TABLE(pos+0x23) = (data&0x0c);

	CACHED_ATTR_TABLE(pos+0x40) = 
	CACHED_ATTR_TABLE(pos+0x41) =
	CACHED_ATTR_TABLE(pos+0x60) = 
	CACHED_ATTR_TABLE(pos+0x61) = (data&0x30) >> 2;

	CACHED_ATTR_TABLE(pos+0x42) = 
	CACHED_ATTR_TABLE(pos+0x43) =
	CACHED_ATTR_TABLE(pos+0x62) = 
	CACHED_ATTR_TABLE(pos+0x63) = (data&0xc0) >> 4;
}

