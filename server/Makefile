#--------------------------------------------------------------------------------
# Program name
#--------------------------------------------------------------------------------
PROGRAM=server



#--------------------------------------------------------------------------------
# Compiler flags
#--------------------------------------------------------------------------------
CXX      = g++
CXXFLAGS = -std=c++14
EXTRA    = -Wall -Wextra -D$(firstword $(subst -, ,$(subst ., ,$(shell hostname))))



#--------------------------------------------------------------------------------
# Project files
#--------------------------------------------------------------------------------
SRCS = $(notdir $(wildcard src/*.cpp))
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SRCS)))
DEPS = $(wildcard ../common/*.cpp)



#--------------------------------------------------------------------------------
# Project configs
#--------------------------------------------------------------------------------
INCS = -Iinclude/ -I../
LIBS = -pthread

EXEDIR = bin



#--------------------------------------------------------------------------------
# PMLIB configs
#--------------------------------------------------------------------------------
# INCS +=  -I${PMLIB_DIR}/
# EXTRA += -DPMLIB 
# LIBS := ${PMLIB_DIR}/pmlib.a $(LIBS)





#--------------------------------------------------------------------------------
# RELEASE build settings
#--------------------------------------------------------------------------------
RELDIR = obj/release
RELEXE = $(EXEDIR)/$(PROGRAM).x
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELDEPS = $(patsubst ../common/%,../common/release/%,$(patsubst %.cpp,%.o,$(DEPS)))
RELCXXFLAGS = -O3
RELINCS =
RELLIBS =



#--------------------------------------------------------------------------------
# DEBUG build settings
#--------------------------------------------------------------------------------
DBGDIR = obj/debug
DBGEXE = $(EXEDIR)/$(PROGRAM)-d.x
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGDEPS = $(patsubst ../common/%,../common/debug/%,$(patsubst %.cpp,%.o,$(DEPS)))
DBGCXXFLAGS = -O0 -ggdb3 -DDEBUG
DBGINCS =
DBGLIBS =



#--------------------------------------------------------------------------------
# INSTRUMENTATION build settings
#--------------------------------------------------------------------------------
INSTRDIR = obj/instrumentation
INSTREXE = $(EXEDIR)/$(PROGRAM)-i.x
INSTROBJS = $(addprefix $(INSTRDIR)/, $(OBJS))
INSTRDEPS = $(patsubst ../common/%,../common/instrumentation/%,$(patsubst %.cpp,%.o,$(DEPS)))
INSTRCXXFLAGS = -DINSTR
INSTRINCS = -I$(EXTRAE_INC)
INSTRLIBS = -L$(EXTRAE_LIB) -lpttrace



#--------------------------------------------------------------------------------
# RULES
#--------------------------------------------------------------------------------


$(info *********************************)
$(info ************  SERVER ************)
$(info *********************************)


.PHONY: all clean prep remake release debug instrument


# Default build
all: prep release debug instrument

#--------------------------------------------------------------------------------
# DEBUG rules
#--------------------------------------------------------------------------------
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS) $(DBGDEPS)
	$(CXX) $(EXTRA) $(CXXFLAGS) $(DBGCXXFLAGS) -o $(DBGEXE) $^ $(LIBS) $(DBGLIBS)

$(DBGDIR)/%.o:  src/%.cpp
	- $(CXX) -c $(CXXFLAGS) $(DBGCXXFLAGS) $(INCS) $(DBGINCS) $(EXTRA) -o $@ $<



#--------------------------------------------------------------------------------
# RELEASE rules
#--------------------------------------------------------------------------------
release: $(RELEXE)

$(RELEXE): $(RELOBJS) $(RELDEPS)
	$(CXX) $(EXTRA) $(CXXFLAGS) $(RELCXXFLAGS) -o $(RELEXE) $^ $(LIBS) $(RELLIBS)

$(RELDIR)/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $(RELCXXFLAGS) $(INCS) $(RELINCS) $(EXTRA) -o $@ $<


#--------------------------------------------------------------------------------
# INSTRUMENTATION rules
#--------------------------------------------------------------------------------
instrument: $(INSTREXE)

$(INSTREXE): $(INSTROBJS) $(INSTRDEPS)
	$(CXX) $(EXTRA) $(CXXFLAGS) $(INSTRCXXFLAGS) -o $(INSTREXE) $^ $(LIBS) $(INSTRLIBS)

$(INSTRDIR)/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INSTRCXXFLAGS) $(INCS) $(INSTRINCS) $(EXTRA) -o $@ $<



#--------------------------------------------------------------------------------
# Other rules
#--------------------------------------------------------------------------------
prep:
	@mkdir -p $(DBGDIR) $(RELDIR) $(INSTRDIR)

remake: clean all

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS) $(INSTREXE) $(INSTROBJS)
