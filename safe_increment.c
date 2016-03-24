#include <stdio.h>
#include <stdlib.h>

#define TURNBIT 0
#define SETFLAGBIT 1

void increment(int, char*);
void enter_region(void);
void leave_region(void);
int get_set_flag(pid_t);
int get_turn(pid_t);

pid_t currentProcess;
pid_t otherProcess;

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Please provide correct arguments");
		return 1;
	}

	FILE *fp;
	fp = fopen("config.txt", "a+");

	currentProcess = getpid();
	fprintf(fp, "%d\n", currentProcess);

	int read;
	int numberOfProcesses = 0;
	while(numberOfProcesses < 2) {
		fseek(fp, 0, SEEK_SET);
		numberOfProcesses = 0;	
		while(fscanf(fp, "%d", &read) == 1) {
			if(read != getpid()) {
				otherProcess = read;
			}
			numberOfProcesses++;
		}
	}

	fclose(fp);

	int numberOfRowsToAppend = atoi(argv[1]);
	char *fileName = argv[2];

	int i;
	for(i=0; i<numberOfRowsToAppend; i++) {
		enter_region();
		increment(numberOfRowsToAppend, fileName);
		leave_region();
	}

	return 0;
}

void increment(int numberOfRowsToAppend, char *fileName) {
	FILE *fp;
	int read;

	fp = fopen(fileName, "r+");
	while(!feof(fp)) {
		fscanf(fp, "%d", &read);
	}
	fclose(fp);

	fp = fopen(fileName, "a+");
	fprintf(fp, "%d\n", read+1);
	fclose(fp);
}

void enter_region()
{
	int status;
	int sv;

	sv = 1;
	sv |= 1 << SETFLAGBIT; 
	sv |= 1 << TURNBIT; 

	int turn;
	if (currentProcess < otherProcess) {
		turn = 1;
	} else {
		turn = 0;
	}
	while (get_set_flag(otherProcess) == 1 && get_turn(otherProcess) == turn);
}

int get_set_flag(pid_t PID)
{
	int status;
	int sv;
	int bit;

	sv = get_sv(PID, &status);	
	bit = (sv & (1 << SETFLAGBIT));
	return bit;
}

int get_turn(pid_t otherPID)
{
	int status;
	int currentProcessSV;
	int currentProcessTurnBit;
	int otherProcessSV;
	int otherProcessTurnBit;

	currentProcessSV = get_sv(getpid(), &status);
	currentProcessTurnBit = (currentProcessSV & (1 << TURNBIT));

	otherProcessSV = get_sv(otherPID, &status);
	otherProcessTurnBit = (otherProcessSV & (1 << TURNBIT));

	if ((currentProcessTurnBit ^ otherProcessTurnBit) == 0) {
		if (currentProcess < otherProcess) {
			int sv, status;
			sv = 1;
			sv |= 1 << SETFLAGBIT; 
			sv |= 0 << TURNBIT; 
			set_sv(sv, &status);
			return 0;
		} else {
			int sv, status;
			sv = 1;
			sv |= 1 << SETFLAGBIT; 
			sv |= 0 << TURNBIT; 
			set_sv(sv, &status);
			return 0;
		}
	}

	return 1;
}

void leave_region()
{
	int status;
	set_sv(0, &status);	
}