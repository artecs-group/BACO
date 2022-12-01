#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <cstdio>

void DIE(const char *str );
unsigned long long getTimeStamp();

void printBinary(int number, FILE* fd, int numDigits=10, bool endl=true, bool decimal=false);
unsigned int toBinary(int number);
int countOnes(int number);

int stepInteger(int number, int mod, int offset=0);


#ifdef DEBUG
#define ONLY_DBG(f) f;
#else
#define ONLY_DBG(f) ;
#endif

#if defined(INSTR) && !defined(INSTRUMENTATION)
#define INSTRUMENTATION 1
#endif
#if defined(INSTRUMENTATION) && !defined(INSTR)
#define INSTR 1
#endif

#ifdef INSTR
#define ONLY_INSTR(f) f;
#else
#define ONLY_INSTR(f) ;
#endif


#if defined(pmlib) && !defined(PMLIB)
#define PMLIB 1
#endif

#ifdef PMLIB
#define ONLY_PMLIB(f) f;
#else
#define ONLY_PMLIB(f) ;
#endif





#ifdef DEBUG
#include <cassert>
#endif


#endif //_UTILS_HPP_
