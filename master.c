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
void ResetNumbers();
void MethodOne(int, int, int);
void MethodTwo(int, int,int );
char * GetString(int, char*);
void RelaxTheCells();
void PrintArray();
int GetBinZero();
void IncrementTime(int);
int GetNanoTime(int);
int GetClockTime(int);
int main(int argc, char ** argv)
{

	int numElem = 8;
	//	int numElem = 64
	int getNumberOfPairs;
	int shared_id1 = 0;	
	int method1Summation = 0;
	int method2Summation = 0;	
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

	
	GenerateRandomNumbers(numElem);
	getNumberOfPairs = ReadInputFile();
        
	printf("Start Method1\n");
	shared_id1 = GetInputPlaceInSharedMem(getNumberOfPairs);
	MethodOne(numElem,2, shared_id1); //method 1;
	method1Summation = GetBinZero(shared_id1);
	//PrintArray(numElem, shared_id1);

	
	printf("Start Method2\n");
	int binSize = ceil(getNumberOfPairs/(log10(getNumberOfPairs)/log10(2)));
	ResetNumbers(shared_id1, getNumberOfPairs);
	MethodTwo(numElem, binSize, shared_id1);
	method2Summation = GetBinZero(shared_id1);
	//PrintArray(numElem, shared_id1);


	printf("Method 1 %d and Method 2 %d \n", method1Summation, method2Summation);	


	if(shmctl(shared_id1, IPC_RMID, NULL) < 0)
		fprintf(stderr, "Shared memory was not deallocated: remove it manually\n");
	
	sem_destroy(mutex);	
	return 0;
}

void PrintArray(int size, int shared_id)
{
        int * arr = (int*)shmat(shared_id, NULL, 0);
        int x = 0;
        for(x = 2; x < size+2; x++)
        {
                printf("%d ", arr[x]);
        }
        printf("\n");
        shmdt(arr);
}

int GetBinZero(int shared_id)
{
	int returnInt;
	int * arr = (int*)shmat(shared_id, NULL, 0);
	returnInt = arr[2];
	shmdt(arr);
	return returnInt;
}

void IncrementTime(int shared_id)
{
	int * arr = (int*)shmat(shared_id, NULL, 0);
	arr[0] = arr[0] + 10000;
	if(arr[0] > 10000000)
	{
		arr[1] = arr[1] + 1;
		arr[0] = 0;
	}		
	shmdt(arr);	
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


void MethodTwo(int size, int binSize, int shared_id)
{

	int maxAllowed = 20;
	int aliveChilds = 0;
	int tempSize = size;
	int bins = 0;
	int pid = 0;
	int pida = 0;
	int anotherTemp = 0;
	int intExec = 0;
	int status = 0;
	int k = 0;
	int i = 0;
	printf("Method Two binSize %d\n",binSize);
        char ** argToPass = malloc(sizeof(char *) * 2);
        for(i = 0; i < 3; i++)
                argToPass[i] = malloc(sizeof(int) * 8);
        i = 0;

	do
	{
		if(maxAllowed > aliveChilds && bins < ceil(size/(binSize*1.0)))
		{	
			pid = fork();
			if(pid < 0)
			{
				perror("Creation of child Process was unsuccessful\n");
			}
			else if(pid == 0)
			{
				if(tempSize > binSize)
				{
					anotherTemp = binSize;
				}
				else
					anotherTemp = tempSize;
				
				
			        printf("Creation of child process was successful %d\n", getpid());
                                char * st;
                                st = (char*)malloc(sizeof(char) * 10);
                                sprintf(st, "%d", ((bins*binSize)));
                                strcpy(argToPass[0], GetString(strlen(st), st));
                                sprintf(st, "%d", anotherTemp);
                                strcpy(argToPass[1], GetString(strlen(st), st));
                                sprintf(st, "%d", shared_id);
                                strcpy(argToPass[2], GetString(strlen(st), st));
                                argToPass[3] = NULL;
                                intExec = execv("bin_adder",argToPass);
                                printf("Exec %d\n", intExec);
                                exit(0);
			}
			else
			{
				printf("Parent sent off child to add two numbers %d and the bin is %d\n", pid, bins);
				bins++;
				tempSize = tempSize - binSize;
				aliveChilds++;			
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
 		IncrementTime(shared_id);                               
	}while(aliveChilds > 0);
	RelaxTheCells(bins, binSize, shared_id);	
	MethodOne(bins, 2, shared_id);

}

void MethodOne(int size,int binSize, int shared_id)
{
	int k = 0;
	int pid;
	int tempSize = size;
	int bins = 0;
	int maxAllowed = 20;
	int aliveChilds = 0;
	int intExec = 0;
	int pida = 0;
	int status = 0;
	int exitFlag = 0;
	int innerExitFlag = 0;
	int anotherTemp = 0;
	char ** argToPass = malloc(sizeof(char *) * 2);
	int i = 0;
	for(i = 0; i < 3; i++)
		argToPass[i] = malloc(sizeof(int) * 8);
	i = 0;

	do
	{			
		//printf("maxAllowed %d aliveChilds %d tempSize %d\n",maxAllowed, aliveChilds, tempSize);
		do
		{
		 
			if(maxAllowed > aliveChilds && tempSize > 0)
			{		
				pid = fork();
				if(pid < 0)
				{
					perror("Creation of child process was unsuccessful\n");
				}
				else if(pid == 0)
				{
	
       		 		        if(tempSize > binSize)
		                        {
       	                                	anotherTemp = binSize;
	                                }
	                                else
	                                	anotherTemp = tempSize;
	
					printf("Creation of child process was successful %d\n", getpid());
					char * st;
					st = (char*)malloc(sizeof(char) * 10);
					sprintf(st, "%d", (bins *binSize));
					strcpy(argToPass[0], GetString(strlen(st), st));
					sprintf(st, "%d", anotherTemp);
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
					printf("Parent sent off child to add two numbers %d and the bin is %d\n", pid, bins);
		 	                bins++;
		                	tempSize = tempSize - binSize;
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
					if(tempSize <= 0 && aliveChilds == 0)
						innerExitFlag = 1;
				}
			}
			IncrementTime(shared_id);
		}while(innerExitFlag == 0 && aliveChilds > 0);
		//PrintArray(size, shared_id);

		if(tempSize <= 0)
		{	
			
			RelaxTheCells(bins, 2, shared_id);
			innerExitFlag = 0;
			tempSize = bins;
			if(bins == 1)
			{
				exitFlag = 1;		
				innerExitFlag = 1;
			}
			bins = 0;
		}
	}while(innerExitFlag == 0 && exitFlag == 0);

}
void RelaxTheCells(int bins, int binSize, int id)
{
	int * arr = (int*)shmat(id, NULL, 0);
	int i;
	for(i = 0; i <bins; i++)
	{	
		arr[i+2] = arr[(i*binSize)+2];
	}	
	shmdt(arr);
}



int GetInputPlaceInSharedMem(int num)
{
	int ret = 0;
	int i = 0;
	int  intVar = 0;
	key_t key = ftok(sharedKey, sharedInt);
	int shmid = shmget(key, (num + 2) * sizeof(int), 0666|IPC_CREAT);
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
	arr[0] = 0;
	arr[1] = 0;
	for(i = 2;ret != EOF && i < num+2; i++)
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

void ResetNumbers(int id, int num)
{
	int i = 0;
	int intVar = 0;
	int * arr = (int*)shmat(id, NULL, 0);
	FILE * fptr;
	if((fptr = fopen(inputFile, "r")) == NULL)
	{
		perror("file problems\n");
		exit(1);
	}
	fscanf(fptr, "%d", &intVar);
	for(i = 2; i < num+2; i++)
	{
		arr[i] = intVar;
		fscanf(fptr, "%d", &intVar);
	}
	fclose(fptr);
	shmdt(arr);

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
