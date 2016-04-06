
OUT_FILE=bbb
OUT=-o $(OUT_FILE)

TARGET_HOST = arm

export CROSS_COMPILE = arm-linux-gnueabi-

AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC	= $(CROSS_COMPILE)gcc
CPP	= $(CC) -E
CXX	= $(CROSS_COMPILE)g++
AR	= $(CROSS_COMPILE)ar
NM	= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump

LIBS = $(LIBBASE) $(LIBHOST) $(LIBBASE) 


INC_FILE=$(wildcard ./*.h)

SRC_FILE=$(wildcard ./*.cpp)

OBJ_FILE=$(patsubst %.cpp, %.o,$(SRC_FILE))

INCLUDE=-I. -I./include/linux -I./include/can

CC_OPTS=$(INCLUDE) -c -static -Wall -Werror -fomit-frame-pointer -Dlinux_gnu -DLINUX -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
CC_OPTS+= -g -DTARGETMIPS -mlong-calls
CC_OPTS+=$(CFLAG_COM)

LINK_OPTS= -static -lc -lrt -lm -lstdc++ -lpthread -lrt 

all: $(OBJ_FILE)
	$(CXX) $(OUT) $(OBJ_FILE) $(LIBDIR) $(LIBS) $(LINK_OPTS)
	$(STRIP) $(OUT_FILE)

$(OBJ_FILE): %.o : %.cpp $(INC_FILE) ./Makefile
	$(CXX) $(CC_OPTS) $(OPTS) -o $@ $<

clean:
	rm -f $(OUT_FILE)
	rm -f $(OBJ_FILE)


