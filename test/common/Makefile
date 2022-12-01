#----------------------------------------------------------------------
CC = g++
CFLAGS=
DBGFLAGS=-ggdb3 -DDEBUG

INCS=-I../../common/
LIBS=

EXTRA = -std=c++11 -Wall -Wextra -D$(shell hostname)
#----------------------------------------------------------------------

SOURCES := $(wildcard *.cpp)
EXES := $(patsubst %.cpp,%.x,$(SOURCES))

all: $(EXES)

debug: CFLAGS += DBGFLAGS
debug: all


%.x: %.cpp $(DEPS_OBJ)
	$(CC) $(EXTRA) $(INCS) $< ../../common/debug/dynamic_bitset.o  -o $@ $(LIBS) 



.PHONY: clean
clean:
	-rm *.x

