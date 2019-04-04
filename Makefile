ARCH = Haiku
CC = gcc

NES_SRC = $(wildcard ./NES/*.cc)
CPU_SRC = ./M6502/M6502.c
APU_SRC = $(wildcard ./SEALAPU/*.cc)
GUI_SRC = $(wildcard ./BeOS/*.cc)
MAPPER_SRC = $(wildcard ./Mappers/*.cc)

NES_OBJ = $(addsuffix .o, $(basename $(NES_SRC)))
CPU_OBJ = $(addsuffix .o, $(basename $(CPU_SRC)))
APU_OBJ = $(addsuffix .o, $(basename $(APU_SRC)))
GUI_OBJ = $(addsuffix .o, $(basename $(GUI_SRC)))
MAPPER_OBJ = $(addsuffix .o, $(basename $(MAPPER_SRC)))

SRCS = $(NES_SRC) $(CPU_SRC) $(APU_SRC) $(GUI_SRC) $(MAPPER_SRC)
OBJS = $(NES_OBJ) $(CPU_OBJ) $(APU_OBJ) $(GUI_OBJ) $(MAPPER_OBJ)

SEAL_DIR = ./SEAL
SEAL_LIB = libaudio.a
LIB_DIR = $(SEAL_DIR)/lib/$(ARCH)

CFLAGS = -O3 -I. -I./SEAL/include
LFLAGS = -L$(LIB_DIR) 
LIBS = -lbe -ldevice -lgame -ltracker -laudio -lmidi -lmedia

all: $(LIB_DIR)/$(SEAL_LIB) BeNES

$(LIB_DIR)/$(SEAL_LIB):
	@if [ ! -e $(SEAL_DIR) ]; then git clone http://github.com/yamagata-makoto/SEAL.git; fi
	cd $(SEAL_DIR)/src; make `echo $(ARCH) | tr A-Z a-z`

BeNES: $(OBJS)
	$(CC) -o BeNES $(OBJS) $(LFLAGS) $(LIBS)

.cc.o: 
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(SEAL_DIR) $(OBJS) ./BeNES

