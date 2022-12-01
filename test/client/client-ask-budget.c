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

  /* dynamic_bitset bt; */
  /* retval = SS_startExecution(&bt); */
  /* fprintf(stdout, "client: Iniciada la ejecución con código: %d\n", retval); */

    while(1){
      int n1, n2;
      scanf("%d %d", &n1, &n2);
      float budget2=SS_requestNewBudget( n1, n2);
      fprintf(stdout, "using %d, requested %d  => given %f\n", n1, n2,budget2);
  }
    
  return 0;
}
