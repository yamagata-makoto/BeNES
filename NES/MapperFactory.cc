/*
    BeNES - Nintendo Entertaiment System Emulator for BeOS
    
    * (C) 2000 by makoto yamagata

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "MapperFactory.h"
#include "Mappers.h"

CasetteProtocol* MapperFactory::Create(ROMfile* romfile)
{
	int mapper = romfile->getMapperNumber();
	CasetteProtocol* casette = NULL;

	printf("create mapper %0d...", mapper);
	switch (mapper) {
	case  0: casette = new Mapper000; break;
	case  1: casette = new Mapper001; break;
	case  2: casette = new Mapper002; break;
	case  3: casette = new Mapper003; break;
	case  4: casette = new Mapper004; break;
	case  6: casette = new Mapper006; break;
	case  7: casette = new Mapper007; break;
	case  9: casette = new Mapper009; break;
	case 11: casette = new Mapper011; break;
	case 15: casette = new Mapper015; break;
	case 34: casette = new Mapper034; break;
	default  : casette = NULL; break;
	}
	if (casette) {
		printf("success.\n");
	}
	else {
		printf("fail.\n");
	}
	return casette;
}

