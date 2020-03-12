#include "myGlobal.h"

void waitRandom();
void writeFile();
int main(int argc, char ** argv)
{
	
	int i;
	int index = 0;
	int size = 0;
	int shid = 0;
	sscanf(argv[0], "%d", &index);
	sscanf(argv[1], "%d", &size);
	sscanf(argv[2], "%d", &shid);

	int * arr = (int*) shmat(shid, NULL, 0);
	int total  = 0;
	//	printf("INDEX %d ",index);
	for(i = 0; i < size; i++)
	{
		total = total + arr[index + i];
	}
	//printf("Total %d\n", total);
	arr[index] = total;
	//printf("UPDATE\n");
	//for(i = 0; i < 8; i++)
	//{	
	//	printf("%d ", arr[i]);
	//}
	//printf("\n");
	shmdt(arr);
	sem_t* mutex = sem_open(semaphoreName, O_EXCL, 0666, 63);		
	sem_unlink(semaphoreName);
	int exitFlag = 0;
	i = 0;
	for(i = 0; i < 5; i++)
	{
		waitRandom();
		sem_wait(mutex);
		sleep(1);
		writeFile(size, index, total);
		sem_post(mutex);
	}
	
	sem_close(mutex);	
	return 0;
}

void writeFile(int size, int index, int total)
{
	FILE * fptr;
	if((fptr = fopen(outputFile, "a"))== NULL)
	{
		perror("file problems\n");
		exit(1);
	}	
	else
	{
		fprintf(fptr, "Pid %d Index %d Size %d \n", getpid(), index, size);
	}
	
	fclose(fptr);

}

void waitRandom()
{
	srand(time(0));
	int sleepTime = 0;
	sleepTime = rand() % 4;
	sleep(sleepTime);
}

