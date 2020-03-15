#include "myGlobal.h"

void waitRandom();
void writeFile();

int main(int argc, char ** argv)
{
	int i;
	int index = 0;
	int size = 0;
	struct sembuf sbuf;
	sscanf(argv[0], "%d", &index);
	sscanf(argv[1], "%d", &size);
	
	sharedID = GetSharedIDFromFile();
	semID = GetSemIDFromFile();
//	printf("bins semID %d\n", semID);


	
	//	printf("Shared id %d\n", sharedID);
	arr = (int*) shmat(sharedID, NULL, 0);
	int total  = 0;
	for(i = 0; i < size; i++)
	{
		total = total + arr[index + i +2];
	}
	arr[index + 2] = total;

	shmdt(arr);
	
	//	mutex = sem_open(semaphoreName, O_EXCL, 0666, 1000);		
	double timeVar = 0;
	i = 0;
	int exitFlag = 0;
	for(i = 0; i < 5 && exitFlag == 0; i++)
	{
	
		#ifdef NOTIMETEST
		waitRandom();
		#endif

		arr = (int*)shmat(sharedID, NULL, 0);
		timeVar = arr[0] + arr[1]/1000000.0;	
		fprintf(stderr, "Pid %d is requesting to enter critical section at clock %f seconds \n", getpid(), timeVar);
		shmdt(arr);

		//sem_wait(mutex);
		sbuf.sem_num = 0;
		sbuf.sem_op = -1;
		sbuf.sem_flg = 0;

		semop(semID, &sbuf, 1);

		arr = (int*)shmat(sharedID, NULL, 0);
		timeVar = arr[0] + arr[1]/1000000.0;
		fprintf(stderr, "Pid %d is in critical section at clock %f seconds  \n", getpid(), timeVar);
		shmdt(arr);

		
		//CRITICAL SECTION
		#ifdef NOTIMETEST
		sleep(1);	
		#endif
		writeFile(size, index, total);
		#ifdef NOTTIMETEST
		sleep(1);	
		#endif
		exitFlag = 1;	
		//CRITICAL SECTION

		
		arr = (int*)shmat(sharedID, NULL, 0);
		timeVar = arr[0] + arr[1]/1000000.0;
		fprintf(stderr, "Pid %d is exiting critical section at clock %f seconds \n", getpid(), timeVar);
		shmdt(arr);
		//sem_post(mutex);			
		sbuf.sem_num = 0;
		sbuf.sem_op = 1;
		sbuf.sem_flg = 0;
		semop(semID, &sbuf, 1);
		
	}	
	exitFlag = 0;
		
		
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
int GetSemIDFromFile()
{
	FILE * fptr;
	int num = 0;
	fptr = fopen(semIDFile, "r");
	fscanf(fptr, "%d", &num);
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
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);
	int sleepTime = 0;
	sleepTime = (rand() % 3)+1;
//	printf("Sleep duration %d\n",sleepTime);
	sleep(sleepTime);
	
}

