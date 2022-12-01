#include "client.h"

#include <stdio.h>

int main(){

  int retval;

  retval = SS_initLibrary();
  fprintf(stdout, "client: registered with code: %d\n", retval);
  
  /* retval = SS_finishLibrary(); */
  /* fprintf(stdout, "client: finish library with code: %d\n", retval); */

  while(1);
  
  return 0;
}
