#----------------------------------------------------------------------
CC = g++
CFLAGS=
DBGFLAGS=-ggdb3 -DDEBUG

INCS=-I../../common/ -I../../client/include -I../../
LIBS=-L../../client/lib/debug -lclient -pthread

EXTRA = -std=c++11 -Wall -Wextra -D$(shell hostname)
#----------------------------------------------------------------------

SOURCES := $(wildcard *.c)
EXES := $(patsubst %.c,%.x,$(SOURCES))

DEPS_SOURCE := $(wildcard ../../common/*.hpp)
DEPS_OBJ := $(patsubst %.hpp,%.o,$(DEPS_SOURCE))



all: $(EXES)

debug: CFLAGS += DBGFLAGS
debug: all


%.x: %.c $(DEPS_OBJ)
	$(CC) $(EXTRA) $(INCS) $< $(DEPS_OBJ) -o $@ $(LIBS) 


# all: $(SOURCES)
# 	for dir in $(SOURCES) ; do \
# 	$(CC) $(EXTRA) $(LIBS) $(INCS) $$dir -o $$dir.x ; \
# 	done



.PHONY: clean
clean:
	-rm *.x

