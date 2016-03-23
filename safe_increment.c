#include <stdio.h>
#include <stdlib.h>

#define SETFLAGBIT 2
#define TURNBIT 1

void enter_region(int, int, int);
void increment(char*);
void leave_region(void);

int get_process(int, int);
int get_other_set_flag(int);
int get_turn(int, int, int);

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Please provide correct arguments.\n");
		return 1;
	}

	int numberOfRowsToAppend = atoi(argv[1]);
	char *writingFile = argv[2];
	char *configFile = argv[3];
	
	int currentPID = getpid();
	int otherPID;

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

	int process = get_process(currentPID, otherPID);

	int i;
	for(i=0; i<numberOfRowsToAppend; i++) {
		enter_region(process, currentPID, otherPID);
		increment(writingFile);
		leave_region();
	}

	return 0;
}

int get_process(int currentPID, int otherPID) {
	if (currentPID < otherPID) {
		return 1;
	}

	return 0;
}

void increment(char *fileName) {
	FILE *fp;
	int read;

	fp = fopen(fileName, "r+");
	while(fscanf(fp, "%d", &read) == 1);
	fclose(fp);

	fp = fopen(fileName, "a+");
	printf("%d\n", read+1);
	fprintf(fp, "%d\n", read+1);
	fclose(fp);
}

void enter_region(int process, int currentPID, int otherPID)
{
	int status;
	int sv;

	sv = 1;
	sv |= 1 << SETFLAGBIT;
	sv |= 1 << TURNBIT; 

	set_sv(sv, &status);
	while (get_other_set_flag(otherPID) == 1 &&
		get_turn(process, currentPID, otherPID) != process);
}

int get_other_set_flag(int otherPID)
{
	int status;
	int otherSV = get_sv(otherPID, &status);
	int otherFlag = !!(otherSV & (1 << SETFLAGBIT));
	return otherFlag;
}

int get_turn(int process, int currentPID, int otherPID)
{
	int status;
	int currentProcessSV = get_sv(currentPID, &status);
	int currentProcessTurn = !!(currentProcessSV & (1 << TURNBIT));
	int otherProcessSV = get_sv(otherPID, &status);
	int otherProcessTurn = !!(currentProcessSV & (1 << TURNBIT));

	if (process == 0) {
		while((currentProcessTurn ^ otherProcessTurn) == 0) {
			currentProcessSV ^= 1 << TURNBIT;
			set_sv(currentProcessSV, &status);
		}
	} else {
		while((currentProcessTurn ^ otherProcessTurn) == 1) {
			currentProcessSV ^= 1 << TURNBIT;
			set_sv(currentProcessSV, &status);
		}
	}
	
	return (currentProcessTurn ^ otherProcessTurn);
}

void leave_region()
{
	int status;
	set_sv(0, &status);	
}
