#include <stdio.h>
#include <stdlib.h>

#define TURNBIT 1
#define SETFLAGBIT 2

void increment(char*);
void enter_region(void);
void leave_region(void);
int get_set_flag(pid_t);
int get_turn(pid_t);

pid_t currentProcess;
pid_t otherProcess;

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Please provide correct arguments");
		return 1;
	}

	int numberOfRowsToAppend = atoi(argv[1]);
	char *writingFile = argv[2];
	char *configFile = argv[3];

	FILE *fp;
	fp = fopen(configFile, "a+");

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

	int i;
	for (i=0; i<numberOfRowsToAppend; i++) {
		enter_region();
		increment(writingFile);
		leave_region();
	}

	return 0;
}

void increment(char *fileName) {
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

	sv = 0;
	sv |= 1 << SETFLAGBIT; 
	sv |= 1 << TURNBIT; 

	int turn;
	if (currentProcess < otherProcess) {
		turn = 0;
	} else {
		turn = 1;
	}

	set_sv(sv, &status);
	sleep(1);
	while ((get_set_flag(otherProcess) != 0) && (get_turn(otherProcess) == turn));
}

int get_set_flag(pid_t PID)
{
	int status;
	int sv;
	int bit;

	sv = get_sv(PID, &status);
	sleep(1);
	bit = (sv & SETFLAGBIT);
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
	sleep(1);
	currentProcessTurnBit = (currentProcessSV & (1 << TURNBIT));

	otherProcessSV = get_sv(otherPID, &status);
	sleep(1);
	otherProcessTurnBit = (otherProcessSV & (1 << TURNBIT));

	return (currentProcessTurnBit ^ otherProcessTurnBit);
}

void leave_region()
{
	int status;
	set_sv(0, &status);	
	sleep(1);
}