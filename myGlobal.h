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
	extern int arr;
	void sig_handler(int);
	void GenerateRandomNumbers(int);
	int ReadInputFile();
	int GetInputPlaceInSharedMem(int);
	const char inputFile[] = "./InputNumberFile.txt";
	const char shared_key[] = "./master.c";
	const int shared_int = 63;
	
#endif	
