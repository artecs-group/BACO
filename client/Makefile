#--------------------------------------------------------------------------------
# Program name
#--------------------------------------------------------------------------------
PROGRAM=libclient



#--------------------------------------------------------------------------------
# Compiler flags
#--------------------------------------------------------------------------------
CXX      = g++
CXXFLAGS = -std=c++11 -fPIC
EXTRA    = -Wall -Wextra $(firstword $(subst ., ,-D$(shell hostname)))

AR       = ar
ARFLAGS  = -cvq


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
LIBS = 

EXEDIR = lib



#--------------------------------------------------------------------------------
# RELEASE build settings
#--------------------------------------------------------------------------------
RELDIR     = obj/release
RELEXEDIR  = $(EXEDIR)/release
RELSTATIC  = $(RELEXEDIR)/$(PROGRAM).a
RELDYNAMIC = $(RELEXEDIR)/$(PROGRAM).so
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELDEPS = $(patsubst ../common/%,../common/release/%,$(patsubst %.cpp,%.o,$(DEPS)))
RELCXXFLAGS = -O3
RELINCS =
RELLIBS =



#--------------------------------------------------------------------------------
# DEBUG build settings
#--------------------------------------------------------------------------------
DBGDIR      = obj/debug
DBGEXEDIR   = $(EXEDIR)/debug
DBGSTATIC   = $(DBGEXEDIR)/$(PROGRAM).a
DBGDYNAMIC  = $(DBGEXEDIR)/$(PROGRAM).so
DBGOBJS     = $(addprefix $(DBGDIR)/, $(OBJS))
DBGDEPS     = $(patsubst ../common/%,../common/debug/%,$(patsubst %.cpp,%.o,$(DEPS)))
DBGCXXFLAGS = -O0 -ggdb3 -DDEBUG
DBGINCS =
DBGLIBS =



#--------------------------------------------------------------------------------
# INSTRUMENTATION build settings
#--------------------------------------------------------------------------------
INSTRDIR      = obj/instrumentation
INSTREXEDIR   = $(EXEDIR)/instrumentation
INSTRSTATIC   = $(INSTREXEDIR)/$(PROGRAM).a
INSTRDYNAMIC  = $(INSTREXEDIR)/$(PROGRAM).so
INSTROBJS     = $(addprefix $(INSTRDIR)/, $(OBJS))
INSTRDEPS     = $(patsubst ../common/%,../common/instrumentation/%,$(patsubst %.cpp,%.o,$(DEPS)))
INSTRCXXFLAGS = -DINSTR
INSTRINCS     = -I$(EXTRAE_INC)
INSTRLIBS     = -L$(EXTRAE_LIB) -lpttrace



#--------------------------------------------------------------------------------
# RULES
#--------------------------------------------------------------------------------


$(info *********************************)
$(info ************  CLIENT ************)
$(info *********************************)


.PHONY: all clean prep remake release debug instrument


# Default build
all: prep release debug instrument

#--------------------------------------------------------------------------------
# DEBUG rules
#--------------------------------------------------------------------------------
debug: $(DBGSTATIC) $(DBGDYNAMIC)

$(DBGSTATIC): $(DBGOBJS) $(DBGDEPS)
	$(AR) $(ARFLAGS) $(DBGSTATIC) $^

$(DBGDYNAMIC): $(DBGOBJS) $(DBGDEPS)
	$(CXX) $(EXTRA) $(CXXFLAGS) $(DBGCXXFLAGS) -shared -o $(DBGDYNAMIC) $^ $(LIBS) $(DBGLIBS)

$(DBGDIR)/%.o:  src/%.cpp
	$(CXX) -c $(CXXFLAGS) $(DBGCXXFLAGS) $(INCS) $(DBGINCS) -o $@ $<



#--------------------------------------------------------------------------------
# RELEASE rules
#--------------------------------------------------------------------------------
release: $(RELSTATIC) $(RELDYNAMIC)

$(RELSTATIC): $(RELOBJS) $(RELDEPS)
	$(AR) $(ARFLAGS) $(RELSTATIC) $^

$(RELDYNAMIC): $(RELOBJS) $(RELDEPS)
	$(CXX) $(EXTRA) $(CXXFLAGS) $(RELCXXFLAGS) -shared -o $(RELDYNAMIC) $^ $(LIBS) $(RELLIBS)

$(RELDIR)/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $(RELCXXFLAGS) $(INCS) $(RELINCS) -o $@ $<


#--------------------------------------------------------------------------------
# INSTRUMENTATION rules
#--------------------------------------------------------------------------------
instrument: $(INSTRSTATIC) $(INSTRDYNAMIC)

$(INSTRSTATIC): $(INSTROBJS) $(INSTRDEPS)
	$(AR) $(ARFLAGS) $(INSTRSTATIC) $^

$(INSTRDYNAMIC): $(INSTROBJS) $(INSTRDEPS)
	$(CXX) $(EXTRA) $(CXXFLAGS) $(INSTRCXXFLAGS) -shared -o $(INSTRDYNAMIC) $^ $(LIBS) $(INSTRLIBS)

$(INSTRDIR)/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INSTRCXXFLAGS) $(INCS) $(INSTRINCS) -o $@ $<



#--------------------------------------------------------------------------------
# Other rules
#--------------------------------------------------------------------------------
prep:
	@mkdir -p $(DBGDIR) $(RELDIR) $(INSTRDIR) $(DBGEXEDIR) $(RELEXEDIR) $(INSTREXEDIR)

remake: clean all

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS) $(INSTREXE) $(INSTROBJS)
