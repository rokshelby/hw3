#include "myGlobal.h"

int main(int argc, char ** argv)
{
	int index = 0;
	int size = 0;
	int shid = 0;
	printf("argc %d\n", argc);
	sscanf(argv[0], "%d", &index);
	sscanf(argv[1], "%d", &size);
	sscanf(argv[2], "%d", &shid);

	int * arr = (int*) shmat(shid, NULL, 0);
	printf("index %d size %d id %d\n", index, size, shid);
	arr[index] = arr[index] + arr[index + size];
	shmdt(arr);

	return 0;
}

