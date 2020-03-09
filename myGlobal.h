#ifndef MYGLOBAL_H
#define MYGLOBAL_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>	
	void sig_handler(int);
	void GenerateRandomNumbers(int);
	void PerformSummation();
	void printSharedInt();
	int ReadInputFile();
	int GetInputPlaceInSharedMem(int);
	char * GetString(int, char*);	
	extern int arr;

	const char inputFile[] = "./InputNumberFile.txt";
	const char sharedKey[] = "./master.c";
	const int sharedInt = 63;
#endif	
