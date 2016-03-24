#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <unistd.h>
#include "petersons.h"

int main(int argc, char *argv[]) {
	int read, i, process;
	int iterations = atoi(argv[1]);
	pid_t currentProcess, otherProcess;	

	FILE *fp;
	FILE *config;

	if((config = fopen(argv[3], "r+")) != NULL) {
		fseek(config, 0, SEEK_END);
		fprintf(config, "%d\n", getpid());
		
		i = 0;	
		while (i < 2) {
			i = 0;
			fseek(config, 0, SEEK_SET);
			while(fscanf(config, "%d", &read) == 1)
				i += 1;
		}
	
		fseek(config, 0, SEEK_SET);
		fscanf(config, "%d", &read);

		if (read == getpid()) {
			process = 0;
			currentProcess = read;
			fscanf(config, "%d", &read);
			otherProcess = read;
		} else {
			process = 1;
			otherProcess = read;
			fscanf(config, "%d", &read);
			currentProcess = read;
		}
	}


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
	int otherFlag = (sv & (1 << 2));

	return otherFlag;
}

int get_turn(process, currentProcess, otherProcess) {
	int status;
	
	int currentProcessSV = get_sv(currentProcess, &status);
	int currentProcessTurn = (currentProcessSV & (1 << 1));

	int otherProcessSV = get_sv(otherProcess, &status);
	int otherProcessTurn = (otherProcessSV & (1 << 1));

	if (process == 0) {	
		while((currentProcessTurn ^ otherProcessTurn) == 0) {
			set_sv(currentProcessSV ^= 1 << 1, &status);
		}
	} else {
		while((currentProcessTurn ^ otherProcessTurn) == 1) {
			set_sv(currentProcessSV ^= 1 << 1, &status);
		}
	}

	return (currentProcessTurn ^ otherProcessTurn);
}

