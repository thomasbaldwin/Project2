#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <unistd.h>
#include "safe_increment.h"

#define SETFLAG 1
#define TURN 2

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Please provide correct arguments.\n");
		return 1;
	}

	int numberOfRowsToAppend = atoi(argv[1]);
	char *writingFile = argv[2];
	char *configFile = argv[3];

	int process;
	pid_t currentPID = getpid();
	pid_t otherPID;

	FILE *fp;
	fp = fopen(configFile, "a+");
	fprintf(fp, "%d\n", currentPID);

	int read;
	int numberOfProcesses = 0;
	while(numberOfProcesses < 2) {
		fseek(fp, 0, SEEK_SET);
		numberOfProcesses = 0;	
		while(fscanf(fp, "%d", &read) == 1) {
			if(read != currentPID) {
				otherPID = read;
			}
			numberOfProcesses++;
		}
	}
	fclose(fp);

	int i;
	for (i=0; i < numberOfRowsToAppend; i++) {
		enter_region(process, currentPID, otherPID);
		increment(writingFile);
		leave_region();
	}

	return 0;
}

void increment(char *fileName) {
		FILE *fp;		
		fp = fopen(fileName, "r+");
		
		int read;
		while (fscanf(fp, "%d", &read) == 1);
		fclose(fp);

		fp = fopen (fileName, "a+");
		fprintf(fp, "%d\n", read+1);
		fclose(fp);
}

void enter_region(int process, int currentPID, int otherPID) {
	int sv = 1;
	int status;

	set_sv(sv |= 1 << 2, &status);
	set_sv(sv |= 1 << 1, &status);
		
	while ((get_turn(process, currentPID, otherPID) != process) && (get_other_flag(otherPID) == TRUE));
}

void leave_region() {
	int status;
	set_sv(0, &status);
}

int get_other_flag (int otherPID) {
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

