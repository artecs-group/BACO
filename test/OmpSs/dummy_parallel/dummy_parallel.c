#include <stdio.h>
#include <stdlib.h>

#pragma omp task
void task(){
	puts("HOLA");
}


int main(){
	int i;
	for(i=0; i<5; i++)
		task();

#pragma omp taskwait

	return 0;
}
