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


void setSharedID();
int getSharedID();
char* getSharedKey();
int getSharedInt();
char * getSharedInputFile();
void GenerateRandomNumbers(int);
int ReadInputFile();
int GetInputPlaceInSharedMem(int);
void PerformSummation(int, int, int);
char * GetString(int, char*);
int main(int argc, char ** argv)
{

	int numElem = 8;
	//	int numElem = 64
	int getNumberOfPairs;
	int shared_id = 0;	
	int method2Shared_id;
	sem_t * mutex = sem_open(semaphoreName, O_CREAT|O_EXCL, 0666, 63);

	//clear the output file
	FILE * fptr;	
	fptr = 	fopen(outputFile, "w");
	fclose(fptr);
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
	
	//printf("number of elements %d\n", numElem);
	
	GenerateRandomNumbers(numElem);
	getNumberOfPairs = ReadInputFile();

	//printf("print shared_id %d\n", shared_id);
        
	printf("Start Method1\n");
	shared_id = GetInputPlaceInSharedMem(getNumberOfPairs/2);
	PerformSummation(numElem, shared_id, 2); //method 1;
	int * arr = (int*)shmat(shared_id, NULL, 0);
	int x = 0;
	for(x = 0; x < 8; x++)
	{
		printf("%d ", arr[x]);
	}
	printf("\n");
	
	printf("Start Method2\n");
	//shared_id = getInputPlaceInSharedMem(getNumberOfPairs/2);
	//PerformSummation(numElem, shared_id, 2
	printf("new size is %f\n", ceil( log10(getNumberOfPairs) / log10(2)  ));

	int test = ceil(log10(getNumberOfPairs)/log10(2));
	
	printf("The integer is %d\n", test);
	shmdt(arr);











	if(shmctl(shared_id, IPC_RMID, NULL) < 0)
		fprintf(stderr, "coult not deallocate shared memory: remove it manually\n");
	
	sem_destroy(mutex);	
	return 0;
}

void PerformSummation(int numElem, int shared_id, int addSeq)
{
	int k = 0;
	int j = 0;
	int i = 0;
	int pid;
	int size = numElem;
	int maxAllowed = 2;
	int aliveChilds = 0;
	int intExec = 0;
	int pida = 0;
	int status = 0;
	int toAdd = addSeq;
	char ** argToPass = malloc(sizeof(char *) * 2);
	for(i = 0; i < 4; i++)
		argToPass[i] = malloc(sizeof(int) * 8);
	i = 0;
	//for(i = 0; i < (size/2)-1; i++)
	do
	{
		//for(j = 0; j < size; j = j + inc)
		while(j < size/2 && maxAllowed > aliveChilds)
		{
			pid = fork();
			if(pid < 0)
			{
				perror("Creation of child process was unsuccessful\n");
			}
			else if(pid == 0)
			{
				printf("Creation of child process was successful %d\n", getpid());
				char * st;
				st = (char*)malloc(sizeof(char) * 10);
				sprintf(st, "%d", j);
				strcpy(argToPass[0], GetString(strlen(st), st));
				sprintf(st, "%d", toAdd);
				strcpy(argToPass[1], GetString(strlen(st), st));
				sprintf(st, "%d", shared_id);
				strcpy(argToPass[2], GetString(strlen(st), st));
				argToPass[3] = NULL;
				intExec = execv("bin_adder",argToPass);
				printf("Exec %d\n", intExec);
				exit(0);

			}
			else if(pid > 0)
			{

				aliveChilds++;
			
				printf("Parent sent off child to add two numbers %d and J is %d\n", pid, j);
				j=j+1;
			}
			
		}
	
	
		for(k = 0; k < aliveChilds; k++)	
		{
			pida = waitpid(pida, &status, WNOHANG);
			if(pida == -1)
			{
				//perror
			}		
			else if(pida == 0)
			{
				//child still running
			}
			else if(pida > 0)
			{
				printf("child is finished %d\n", pida);
				aliveChilds--;
			}
		}

		if(j == size/2)
		{
			size = size / 2;
			j = 0;
		}

		
			
	}while(aliveChilds > 0 && size >= 0);

}



int GetInputPlaceInSharedMem(int num)
{
	int ret = 0;
	int i = 0;
	int  intVar = 0;
	key_t key = ftok(sharedKey, sharedInt);
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
		printf("%d ", arr[i]);
	}
	printf("\n");
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
char * GetString(int size, char * str)
 {
         int i = 0;
         char * returnStr = malloc(sizeof(char) * strlen(str));
 	 for(i = 0; i < strlen(str); i++)
 	 {
 	 	returnStr[i] = str[i];
         }
 	 returnStr[strlen(str)] = 0;
 	 return returnStr;
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
