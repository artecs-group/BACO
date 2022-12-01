PROGRAM=cholesky

TARGETS=$(PROGRAM)-p $(PROGRAM)-i $(PROGRAM)-d


CC = mcc

CFLAGS = --ompss
CFLAGS_P =
CFLAGS_I = --instrument
CFLAGS_D = --debug -DCHECK_RESULTS -DVERBOSE

MKL_LIB_DIR=$(MKL_ROOT)/lib/intel64/
MKL_INC_DIR=$(MKL_ROOT)/include
LIBS  = --Wl,-L$(MKL_LIB_DIR) -lmkl_sequential -lmkl_core -lmkl_rt -lpthread
INCS  = -I$(MKL_INC_DIR)

EXTRA = -std=c99 -O3 -Wall -Wno-unused

all: $(TARGETS)


$(PROGRAM)-p: $(PROGRAM).c $(MKL_CHECK)
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_P) $(EXTRA) $(INCS) -o $@ $< $(LIBS)


$(PROGRAM)-i: $(PROGRAM).c $(MKL_CHECK)
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_I) $(EXTRA) $(INCS) -o $@ $< $(LIBS)

$(PROGRAM)-d: $(PROGRAM).c $(MKL_CHECK)
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_D) $(EXTRA) $(INCS) -o $@ $< $(LIBS)

clean:
	rm -f $(CC)_* *.o *~ $(TARGETS) 

