#include "client.h"

#include <stdio.h>
#include "utils.hpp"
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>

static void sig_handler(int signo)
{
    if(signo != SIGINT)
	puts("OJO; que la señal capturada no es la que esperabamos");

    SS_finishLibrary();

    exit(EXIT_SUCCESS);
}


int main(){


  signal(SIGINT, sig_handler);
  signal(SIGKILL, sig_handler);
    
    
  int retval;

  retval = SS_initLibrary();
  fprintf(stdout, "client: registered with code: %d\n", retval);

  dynamic_bitset bt;

  if((retval=SS_startExecution(&bt))<0){
      fprintf(stderr, "error al inicair la ejecución\n");
      return -1;
  } else {
      fprintf(stdout, "Iniciando ejecución con máscara %s\n", bt.to_string().c_str());
  }


  while(1){
      sleep(2);
      if((retval=SS_askForNewCpuMask(&bt))<0){
	  fprintf(stderr, "error al solicitar la nueva máscara\n");
	  return -2;
      }
      else
	  fprintf(stdout, "Nueva mascara: %s\n", bt.to_string().c_str());

  }
  

  
  return 0;
}
