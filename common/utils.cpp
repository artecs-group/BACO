#include "utils.hpp"

#include <cstdlib>

#include <sys/time.h>
void DIE(const char *str ) {
    perror( str );			
    exit(EXIT_FAILURE);				
}



unsigned long long getTimeStamp(){
  struct timeval tv;
  if(gettimeofday(&tv, NULL)==-1){
    perror(__FILE__ ": Error obtaining the timestamp");
    return 0;
  }
  else{
    return tv.tv_sec * 1e9 + tv.tv_usec;
  }
}


void printBinary(int number, FILE* fd, int numDigits, bool endl, bool decimal){

  for(int i=numDigits-1; i>=0; i--){
    if((number & (1<<i))!=0)
      fprintf(fd, "1");
    else
      fprintf(fd, "0");
  }

  if(decimal)
      fprintf(fd, " (%d)", number);
  
  if(endl)
    fprintf(fd, "\n");
}


unsigned int toBinary(int number){
  int ret=0;
  int dig=1;
  for(int i=0; i<32; i++){
    if((number&(1<<i)) != 0)
      ret += dig;
    dig *= 10;
  }

  return ret;
}


int countOnes(int number)
{
    int n=0;
    for(int i=0; i<32; i++){
	if((number & (1<<i))!=0) n++;
    }

    return n;    
}

int stepInteger(int number, int mod, int offset)
{
  //interger division power
  return (number / mod) * mod + offset; 
}
