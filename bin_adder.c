#include "myGlobal.h"

void waitRandom();
void writeFile();
int GetNanoTime(int);
int GetClockTime(int);
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
		total = total + arr[index + i + 2];
	}
	//printf("Total %d\n", total);
	arr[index+2] = total;
	//printf("UPDATE\n");
	//for(i = 0; i < 8; i++)
	//{	
	//	printf("%d ", arr[i]);
	//}
	//printf("\n");
	shmdt(arr);
	sem_t* mutex = sem_open(semaphoreName, O_EXCL, 0666, 63);		
	//sem_unlink(semaphoreName);
	i = 0;
	for(i = 0; i < 5; i++)
	{
		waitRandom();
		fprintf(stderr, "Pid %d is requesting to enter critical section at clock %d nano %d \n", getpid(), GetClockTime(shid), GetNanoTime(shid));
		sem_wait(mutex);
		//sleep(6); //gives better adder_log results
		sleep(1);
		fprintf(stderr, "Pid %d is in critical section at clock %d nano %d \n", getpid(), GetClockTime(shid), GetNanoTime(shid));
		writeFile(size, index, total);
		fprintf(stderr, "Pid %d is exiting critical section at clock %d nano %d \n", getpid(), GetClockTime(shid), GetNanoTime(shid));
		sem_post(mutex);
	}
	
	sem_close(mutex);	
	return 0;
}
int GetNanoTime(int shared_id)
{

        int nanoTime = 0;
        int * arr = (int*)shmat(shared_id, NULL, 0);
        nanoTime = arr[0];
        shmdt(arr);
        return nanoTime;
}

int GetClockTime(int shared_id)
{
        int clockTime = 0;
        int * arr = (int*)shmat(shared_id, NULL, 0);
        clockTime = arr[1];
        shmdt(arr);
        return clockTime;
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

