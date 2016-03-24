#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <unistd.h>
#include “safe_increment.h”

#define SETFLAG 1
#define TURN 2

int main(int argc, char *argv[]) {
	int i, process;
	int iterations = atoi(argv[1]);
	pid_t currentProcess = getpid();
	pid_t otherProcess;

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

	for (i=0; i < iterations; i++) {
		enter_region(process, currentProcess, otherProcess);
		fp = fopen(argv[2], "r+");
		while (fscanf(fp, "%d", &read) == 1);
		fclose(fp);

		fp = fopen (argv[2], "a+");
		fprintf(fp, "%d\n", read+1);
		fclose(fp);
		leave_region();
	}

	return 0;
}

void enter_region(int process, int currentProcess, int otherProcess) {
	int sv = 1;
	int status;

	set_sv(sv |= 1 << 2, &status);
	set_sv(sv |= 1 << 1, &status);
		
	while ((get_turn(process, currentProcess, otherProcess) != process) && (get_other_flag(otherProcess) == TRUE));
}

void leave_region() {
	int status;
	set_sv(0, &status);
}

int get_other_flag (int otherProcess) {
	int status;
	int sv = get_sv(otherProcess, &status);
	int otherFlag = (sv & (1 << SETFLAG));

	return otherFlag;
}

int get_turn(process, currentProcess, otherProcess) {
	int status;
	
	int currentProcessSV = get_sv(currentProcess, &status);
	int currentProcessTurn = (currentProcessSV & (1 << TURN));

	int otherProcessSV = get_sv(otherProcess, &status);
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

