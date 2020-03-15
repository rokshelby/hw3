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
int sharedID, semID;
struct itimerval myTime;
struct timeval child, start1;
time_t t;
int main(int argc, char ** argv)
{
	int numElem;
	int method1Summation = 0;
	int method2Summation = 0;
	
	struct timeval stop1, start2, stop2;
	double one,two,three,four;
	
		
	signal(SIGALRM, CatchSignal);
	signal(SIGINT, CatchSignal);
	myTime.it_value.tv_sec = 100;
	myTime.it_value.tv_usec = 0;
	myTime.it_interval = myTime.it_value;
	time_t method1, method2;
	time(&t);
	//sem_t * mutex = sem_open(semaphoreName, O_CREAT|O_EXCL, 0666, 1000);
	semID = semget(1963,1,IPC_CREAT|0666);
	semctl(semID, 0, SETVAL, 1);
	SetSemID(semID);	
	if(semID == -1)
	{
		perror("master, semaphore error\n");
		exit(1);
	}
	//printf("semID %d\n",semID);
	//sem_unlink(semaphoreName);
	//clear the output file
	
	FILE * fptr;	
	fptr = 	fopen(outputFile, "w");
	fclose(fptr);
	//get the number of ints;
	if(argc > 1)
	{
		numElem = ReadArgument(argv[1]);
	}
	else
		numElem = 64;

	GenerateRandomNumbers(numElem);
	ReadInputFile();
	time(&method1);
	printf("Start Method1 at %s \n", ctime(&method1));
	
	sharedID = GetInputPlaceInSharedMem(numElem);
	SetSharedID(sharedID);
	
	setitimer(ITIMER_REAL, &myTime, NULL);

	gettimeofday(&start1, NULL);
	MethodOne(numElem,2); //method 1;
	method1Summation = GetBinZero();
	gettimeofday(&stop1, NULL);
	
	one = (double)(stop1.tv_usec - start1.tv_usec)/ 1000000;
	two = (double)(stop1.tv_sec - start1.tv_sec);



	time(&method2);
	printf("\nStart Method2 at %s \n", ctime(&method2));
	int binSize = ceil(numElem / (log2(numElem)));
	ResetNumbers(numElem);
	
	gettimeofday(&start2, NULL);
	MethodTwo(numElem, binSize);
	gettimeofday(&stop2, NULL);

	method2Summation = GetBinZero();
	

	three = (double)(stop2.tv_usec - start2.tv_usec)/1000000;
	four = (double)(stop2.tv_sec - start2.tv_sec);

	//printf("Method 1 Summation %d completed in %f seconds\n",method1Summation,difftime(method2,method1));
	//printf("Method 2 Summation %d completed in %f seconds\n",method2Summation,difftime(endMethod,method2));
	one = one + two;
	three = three + four;
	printf("Method 1 Summation %d completed in %f seconds\n",method1Summation, one);
	printf("Method 2 Summation %d completed in %f seconds\n",method2Summation, three);

	
	if(shmctl(sharedID, IPC_RMID, NULL)< 0)
		fprintf(stderr, "Shared memory was not deallocated: remove it manually\n");
	
	remove(sharedIDFile);
	remove(inputFile);
	//sem_destroy(mutex);
		
	if((semctl(semID, 0, IPC_RMID)) < 0)
		fprintf(stderr, "Semaphore array was not deallocated: remove it manually\n");
	return 0;
}

void PrintArray(int size)
{
	sharedID = GetSharedIDFromFile();
        arr = (int*)shmat(sharedID, NULL, 0);
        int x = 0;
	printf("Print Array\n");
        for(x = 0; x < size; x++)
        {
                printf("%d ", arr[x]);
        }
        printf("\n");
        shmdt(arr);
}

int ReadArgument(char * str)
{
	int numElem;
        sscanf(str, "%d", &numElem);
	return numElem;
}


void UpdateTime()
{

	gettimeofday(&child, NULL);
	sharedID = GetSharedIDFromFile();
	arr = (int*)shmat(sharedID, NULL, 0);
	arr[0] = (double)(child.tv_sec-start1.tv_sec);
	arr[1] = (double)(child.tv_usec-start1.tv_usec);
	shmdt(arr);

}

void CatchSignal(int sig)
{
	if(sig == SIGINT)
	{
		printf("\nKill signal was executed\n");
	}
	else if(sig == SIGALRM)
	{
		printf("Time has expired\n");
	}
	int i = 0;
	
        if(shmctl(sharedID, IPC_RMID, NULL)< 0)
                fprintf(stderr, "Shared memory was not deallocated: remove it manually\n");

        remove(sharedIDFile);
        remove(inputFile);
            
        if((semctl(semID, 0, IPC_RMID)) < 0)
        	fprintf(stderr, "Semaphore array was not deallocated: remove it manually\n");
      
	for(i = 0; i < 20; i++)
		kill(pids[i], SIGKILL);
	kill(getpid(), SIGKILL);
	exit(0);	
}


int GetBinZero()
{
	int returnInt = 0;
	arr = (int*)shmat(sharedID, NULL, 0);
	//printf("Shared ID %d\n",*shared_id1);
	returnInt = arr[2];
	shmdt(arr);
	return returnInt;
}



void MethodTwo(int size, int binSize)
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
        char ** argToPass = (char**)malloc(sizeof(char *) * 2);
        for(i = 0; i < 2; i++)
                argToPass[i] = (char*)malloc(sizeof(int) * 8);
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

			       // printf("Creation of child process was successful %d\n", getpid());
                                char * st;
                                st = (char*)malloc(sizeof(char) * 10);
                                sprintf(st, "%d", ((bins*binSize)));
                                strcpy(argToPass[0], st);
                                sprintf(st, "%d", anotherTemp);
                                strcpy(argToPass[1], st);
                                argToPass[2] = NULL;
                                intExec = execv("bin_adder",argToPass);
                                printf("Exec %d\n", intExec);
				free(st);
                                exit(0);
			}
			else
			{
				//printf("Parent sent off child to add two numbers %d and the bin is %d\n", pid, bins);
				pids[bins] = pid;
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
                            //q
                            //perror
                        }
                        else if(pida == 0)
                        {
			        //child still running                                                                                                                                                                
			}
                        else if(pida > 0)
			{
                            //printf("child is finished %d\n", pida);
                            aliveChilds--;
                        }
                 }
 		UpdateTime();
	}while(aliveChilds > 0);
	free(argToPass);
	RelaxTheCells(bins, binSize);	
	MethodOne(bins, 2);

}

void MethodOne(int size,int binSize)
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
	char ** argToPass = (char**)malloc(sizeof(char *) * 2);
	int i = 0;
	for(i = 0; i < 2; i++)
		argToPass[i] = (char*)malloc(sizeof(int) * 8);
	i = 0;

	do
	{//	printf("Shared _id %d\n",*shared_id1);	
		//printf("maxAllowed %d aliveChilds %d tempSize %d\n",maxAllowed, aliveChilds, tempSize);
		do
		{
			//printf("maxAllowed, %d aliveChilds %d tempSize %d\n", maxAllowed, aliveChilds, tempSize);			 
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
	
					//printf("Creation of child process was successful %d\n", getpid());
					char * st;
					st = (char*)malloc(sizeof(char) * 8);
					sprintf(st, "%d", (bins *binSize));
					strcpy(argToPass[0], st);
					sprintf(st, "%d", anotherTemp);
					strcpy(argToPass[1], st);
					argToPass[2] = NULL;
					intExec = execv("bin_adder",argToPass);
					printf("Exec %d\n", intExec);
					free(st);
					exit(0);
	
				}
				else if(pid > 0)
				{
					aliveChilds++;
				//	printf("Parent sent off child to add two numbers %d and the bin is %d\n", pid, bins);
					pids[bins] = pid;
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
					//printf("child is finished %d\n", pida);
					aliveChilds--;
					if(tempSize <= 0 && aliveChilds == 0)
						innerExitFlag = 1;
				}
			}
			
			UpdateTime();
		}while(innerExitFlag == 0 && aliveChilds > 0);
		//PrintArray(size, shared_id);

		if(tempSize <= 0)
		{	
			//			printf("other shared id %d\n", share	
			RelaxTheCells(bins, 2);
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
	free(argToPass);

}
void RelaxTheCells(int bins, int binSize)
{
	sharedID = GetSharedIDFromFile();
	arr = (int*)shmat(sharedID, NULL, 0);
	int i;
	for(i = 0; i <bins; i++)
	{	
		arr[i+2] = arr[(i*binSize)+2];
	}	
	shmdt(arr);
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


int GetInputPlaceInSharedMem(int num)
{
	int ret = 0;
	int i = 0;
	int  intVar = 0;
	key_t key = ftok(sharedKey, sharedInt);
	//int shmid = shmget(key, (num + 2) * sizeof(int), 0666|IPC_CREAT);
	int shmid = shmget(key, (num+2)*  sizeof(int), 0666|IPC_CREAT);
	
	//printf("print shmid %d\n", shmid);
	arr = (int*) shmat(shmid,NULL ,0);
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
	for(i = 0;ret != EOF && i < num; i++)
	{
		arr[2+i] = intVar;
		ret = fscanf(fptr, "%d", &intVar);
		printf("%d ", arr[2+i]);
	}
	printf("\n");
	fclose(fptr);
	shmdt(arr);
	return shmid;
}


void SetSharedID(int num)
{
	FILE * fptr;
	if((fptr = fopen(sharedIDFile, "w")) == NULL)
	{
		perror("file problems\n");
		exit(1);
	}
	fprintf(fptr, "%d",num);
	fclose(fptr);
}
void SetSemID(int num)
{
	FILE * fptr;
	if((fptr = fopen(semIDFile, "w")) == NULL)
	{
		perror("semaphore file problems\n");
		exit(1);
	}
	fprintf(fptr, "%d", num);
	fclose(fptr);
}
void ResetNumbers(int num)
{
	int i = 0;
	int intVar = 0;
	arr = (int*)shmat(sharedID, NULL, 0);
	FILE * fptr;
	if((fptr = fopen(inputFile, "r")) == NULL)
	{
		perror("file problems\n");
		exit(1);
	}
	fscanf(fptr, "%d", &intVar);
	for(i = 0; i < num; i++)
	{
		arr[i+2] = intVar;
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
			tempInt = rand() % 256;
			total = total + tempInt;
			fprintf(fptr,"%d\n", tempInt);
		}	
	}
	printf("Summation: %d\n", total);
	fclose(fptr);

} 
