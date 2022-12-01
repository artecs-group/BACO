#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.hpp"

#define MAX_ITS 25

void checkCores(){
  int cores = SS_askForNewCpuMask();
  fprintf(stdout, "La nueva mascara es: ");
  if(cores != -1)
    printBinary(cores, stdout, 10, true);
  else
    fprintf(stdout, "-1\n");
}


int main(int argc, char** argv){
  int its = MAX_ITS;
  if(argc>1)
    its = atoi(argv[1]);
  
  SS_initLibrary();

  for(int i=0; i<its; i++){
    checkCores();
    sleep(1);
  }

  SS_finishLibrary();
  return 0;
}
