//#include "myGlobal.h"
#include <stdlib.h>
#include <stdio.h>

int getSharedID();
void printSharedInt();
extern int rakeem;
int main(int argc, char ** argv)
{
	
	int i = 0;

	printf("The Arguments \n");
	for(i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");



	FILE * fptr;

	fptr = fopen("test.txt", "w");

	fprintf(fptr, "%d", 63);
	fclose(fptr);

	printf("shared id %d\n", getSharedID());
	printf("shared_id %d\n", rakeem);
	printSharedInt();
	return 0;
}

