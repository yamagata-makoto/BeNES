
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

LIB_DIR = ./SEAL/lib/Haiku

CFLAGS = -O3 -I. -I./SEAL/include
LFLAGS = -L$(LIB_DIR) 
LIBS = -lbe -ldevice -lgame -ltracker -laudio -lmidi -lmedia

all: BeNES 

BeNES: $(OBJS)
	$(CC) -o BeNES $(OBJS) $(LFLAGS) $(LIBS)

.cc.o: 
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm $(OBJS) ./BeNES

