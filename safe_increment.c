#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <unistd.h>
#include "safe_increment.h"

#define SETFLAG 2
#define TURN 1

int main(int argc, char *argv[]) {
	int iterations = atoi(argv[1]);
	int process;
	pid_t currentPID;
	pid_t otherPID;	

	FILE *fp;
	fp = fopen(argv[3], "a+");
	fprintf(fp, "%d\n", currentPID);

	int read;
	int numberOfProcesses = 0;
	while(numberOfProcesses < 2) {
		fseek(fp, 0, SEEK_SET);
		numberOfProcesses = 0;	
		while(fscanf(fp, "%d", &read) == 1) {
			if(read == getpid()) {
				process = 0;
				currentPID = read;
			} else {
				process = 1;
				otherPID = read;
			}
			numberOfProcesses++;
		}
	}

	fclose(fp);

	int i;
	for (i=0; i < iterations; i++) {
		enter_region(process, currentPID, otherPID);
		increment(argv[2]);
		leave_region();
	}

	return 0;
}

void enter_region(int process, int currentPID, int otherPID) {
	int sv = 1;
	int status;

	set_sv(sv |= 1 << SETFLAG, &status);
	set_sv(sv |= 1 << TURN, &status);
		
	while ((get_turn(process, currentPID, otherPID) != process) 
		&& (get_other_set_flag(otherPID) == TRUE));
}

void increment(char *fileName) {
	FILE *fp;
	int read;

	fp = fopen(fileName, "r+");
	while (fscanf(fp, "%d", &read) == 1);
	fclose(fp);

	fp = fopen(fileName, "a+");
	fprintf(fp, "%d\n", read+1);
	fclose(fp);
}

void leave_region() {
	int status;
	set_sv(0, &status);
}

int get_other_set_flag(int otherPID) {
	int status;
	int sv = get_sv(otherPID, &status);
	int otherFlag = (sv & (1 << SETFLAG));
	return otherFlag;
}

int get_turn(process, currentPID, otherPID) {
	int status;
	
	int currentProcessSV = get_sv(currentPID, &status);
	int currentProcessTurn = (currentProcessSV & (1 << TURN));

	int otherProcessSV = get_sv(otherPID, &status);
	int otherProcessTurn = (otherProcessSV & (1 << TURN));

	if (process == 0) {	
		while((currentProcessTurn ^ otherProcessTurn) == 0) {
			set_sv(currentProcessSV ^= 1 << TURN, &status);
		}
	} else {
		while((currentProcessTurn ^ otherProcessTurn) == 1) {
			set_sv(currentProcessSV ^= 1 << TURN, &status);
		}
	}

	return (currentProcessTurn ^ otherProcessTurn);
}

