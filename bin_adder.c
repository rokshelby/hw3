#include "myGlobal.h"

void waitRandom();
void writeFile();

int main(int argc, char ** argv)
{
	
	int i;
	int index = 0;
	int size = 0;
	sscanf(argv[0], "%d", &index);
	sscanf(argv[1], "%d", &size);
	
	sharedID = GetSharedIDFromFile();
	//	printf("Shared id %d\n", sharedID);
	arr = (int*) shmat(sharedID, NULL, 0);
	int total  = 0;

	for(i = 0; i < size; i++)
	{
		total = total + arr[index + i +2];
	}
	arr[index + 2] = total;

	shmdt(arr);
	sem_t* mutex = sem_open(semaphoreName, O_EXCL, 0666, 63);		

	i = 0;
	for(i = 0; i < 5; i++)
	{
		#ifdef NOTIMETEST
		
		waitRandom();
		arr = (int*)shmat(sharedID, NULL, 0);
		fprintf(stderr, "Pid %d is requesting to enter critical section at clock %ld seconds and %ld micro seconds  \n", getpid(), arr[0],arr[1]);
		shmdt(arr);
		#endif

		sem_wait(mutex);
		
		#ifdef NOTIMETEST
		sleep(1);
		arr = (int*)shmat(sharedID, NULL, 0);
		fprintf(stderr, "Pid %d is in critical section at clock %ld seconds and %ld micro seconds  \n", getpid(), arr[0],arr[1]);
		shmdt(arr);
		#endif
		
		writeFile(size, index, total);
		
		#ifdef NOTIMETEST
		arr = (int*)shmat(sharedID, NULL, 0);
		fprintf(stderr, "Pid %d is exiting critical section at clock %ld seconds and %ld micro seconds  \n", getpid(), arr[0], arr[1]);
		shmdt(arr);
		#endif
		
		sem_post(mutex);
	}
	
	sem_close(mutex);	
	return 0;
}
int GetSharedIDFromFile()
{
	FILE * fptr;
	int num = 0;
	fptr = fopen(sharedIDFile, "r");
	fscanf(fptr,"%d", &num);
	fclose(fptr);
	return num;
}
void writeFile(int size, int index, int total)
{
	FILE * fptr;
	if((fptr = fopen(outputFile, "ab+"))== NULL)
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

