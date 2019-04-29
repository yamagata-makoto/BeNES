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
#ifndef CPU_H_DEFINED
#define CPU_H_DEFINED

class CPUwriteDelegate
{
public:
  virtual void write4_5(unsigned address, unsigned char data) { ; }
};

class CPUreadDelegate
{
public:
  virtual unsigned char read4_5(unsigned address) { return 0xff; }
};

class CPUprotocol
{
  static CPUprotocol instance;
  CPUprotocol() { ; }

public:
  static CPUprotocol* Instance() { return &(CPUprotocol::instance); }
  void reset();
  void run(int cycles);
  void intNMI();
  void intIRQ();
  void setDelegate(CPUwriteDelegate* aDelegate);
  void setDelegate(CPUreadDelegate* aDelegate);
};

CPUprotocol* CPU();

#endif
