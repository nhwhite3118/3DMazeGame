
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int main(){
	clock_t init, final, current;

	init=clock();
	for(int i = 0; i< 1000; i++)
	{

		printf("%f\n" , (double)(current = clock()-init) / ((double)CLOCKS_PER_SEC) );
	}
	final = clock()-init;
	printf("%f\n" , (double)final / ((double)CLOCKS_PER_SEC) );
}