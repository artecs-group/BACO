#----------------------------------------------------------------------
CC = g++
CFLAGS=
DBGFLAGS=-ggdb3 -DDEBUG

INCS=-I../../common/
LIBS=-pthread

EXTRA = -Wall -Wextra -D$(shell hostname)
#----------------------------------------------------------------------

SOURCES := $(wildcard *.c)
EXES := $(patsubst %.c,%.x,$(SOURCES))

DEPS_SOURCE := $(wildcard ../../common/*.hpp)
DEPS_OBJ := $(patsubst %.hpp,%.o,$(DEPS_SOURCE))



all: $(EXES)

debug: CFLAGS += $(DBGFLAGS)
debug: all


%.x: %.c $(DEPS_OBJ)
	$(CC) $(CFLAGS) $(EXTRA) $(LIBS) $(INCS) $^ -o $@


# all: $(SOURCES)
# 	for dir in $(SOURCES) ; do \
# 	$(CC) $(EXTRA) $(LIBS) $(INCS) $$dir -o $$dir.x ; \
# 	done



.PHONY: clean
clean:
	-rm *.x

