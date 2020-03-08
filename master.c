//rakeem shelby
//hw 3
//edited on march 5, 2020
//the program will have a statically linked library
//it will compute the summation of n integers in two different ways
//the use of semaphores willl be used to protect the critical resources
//an output file will be used to be updated by the child proccesses called adder_log to say what has been done. semaphores will protect the critical resource which is the log file in this case. 
//
//the main process will print a message when an interrupt signal is received all subseqent child processes will be killed
//if the program isn't finished before 100 seconds the program will stop.
// 

#include "myGlobal.h"

int main(int argc, char ** argv)
{

	int numElem = 64;
	int getNumberOfPairs;
	int shared_id = 0;
	
	//get the number of ints;
	if(argc > 1)
	{
		sscanf(argv[1], "%d", &numElem);
		if(numElem < 64)
		{
			numElem = 64;
		}

		if(numElem % 2 != 0)
		numElem++;
		
	}
	printf("number of elements %d\n", numElem);
	GenerateRandomNumbers(numElem);
	getNumberOfPairs = ReadInputFile();
	shared_id = GetInputPlaceInSharedMem(getNumberOfPairs/2);
	
	printf("print shared_id %d\n", shared_id);
       
	
	PerformSummation(numElem, shared_id);

	if(shmctl(shared_id, IPC_RMID, NULL) < 0)
		fprintf(stderr, "coult not deallocate shraed memory: remove it manually\n");
	
	return 0;
}

void PerformSummation(int numElem, int shared_id)
{

	int j = 0;
	int i = 0;
	int num = numElem;
	int inc =2;
	int * arr;
	int pid;
	for(i = 0; i < (size/2)-1; i++)
	{
		for(j = 0; j < size; j = j + inc);
		{

			pid = fork();
			if(pid < 0)
			{
				perror("Creation of child process was unsuccessful\n");
			}
			else if(pid == 0)
			{
				printf("Creationg of child process was successful %d\n", getpid());
				char * st;
				st = (char*)malloc(sizeof(char) * 10);
				sprintf(st, "%d", count);
				strcpy(arg
				execv("bit_adder", 

			}
			

			arr = addArray(arr, j, inc);

		}
		inc = inc * 2;
	}

}


int GetInputPlaceInSharedMem(int num)
{
	int ret = 0;
	int i = 0;
	int  intVar = 0;
	key_t key = ftok(shared_key, shared_int);
	int shmid = shmget(key, (num * 2) * sizeof(int), 0666|IPC_CREAT);
	printf("print shmid %d\n", shmid);
	int * arr = (int*) shmat(shmid,  NULL ,0);
	arr[0] = 0;
	FILE * fptr;
	if((fptr = fopen(inputFile, "r")) == NULL)
	{
		perror("file problems\n");
		exit(1);
	}
	ret = fscanf(fptr, "%d", &intVar);
	for(i = 0;ret != EOF && i < num * 2; i++)
	{
		arr[i] = intVar;
		ret = fscanf(fptr, "%d", &intVar);
	}
	fclose(fptr);
	shmdt(arr);
	return shmid;
}



int ReadInputFile()
{
	int count = 0;
	int * num = (int*)malloc(sizeof(int));
	int total = 0;
	int ret = 0;
	FILE * fptr;
	if((fptr = fopen(inputFile, "r")) == NULL)
	{
		perror("file problems");
		exit(1);
	}
	ret = fscanf(fptr, "%d", num);
	while(ret != EOF)
	{
	
		total = total + *num;
		count++;
		ret = fscanf(fptr, "%d", num);
	}

	printf("Summation: %d total numbers: %d\n", total, count);
	fclose(fptr);
	free(num);
	return count;
}

void GenerateRandomNumbers(int num)
{
	srand(time(0)); //random seed
	int i = 0;
	int total = 0;
	int tempInt = 0;
	FILE * fptr;
	fptr = fopen(inputFile,"w");
	if(fptr == NULL)
	{
		perror("file problems\n");
		exit(1);
	}
	else
	{
		for(i = 0; i < num; i++)
		{
			tempInt = rand() %256;
			total = total + tempInt;
			fprintf(fptr,"%d\n", tempInt);
		}	
	}
	printf("Summation: %d\n", total);
	fclose(fptr);

} 
