#include "myGlobal.h"




int main(int argc, char ** argv)
{
	
	int i = 0;
	fflush(stdout);
	fprintf(stderr,"The Arguments \n");
	for(i = 0; i < argc; i++)
	{
		fprintf(stderr,"%s ", argv[i]);
	}
	fprintf(stderr,"\n");



	FILE * fptr;

	fptr = fopen("test.txt", "w");

	fprintf(fptr, "%d", 63);
	fclose(fptr);







	return 0;
}
