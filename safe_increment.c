#include <stdio.h>
#include <stdlib.h>

int turn;
pid_t currentProcess;
pid_t otherProcess;

void increment(int, char*);
void enter_region(void);
void leave_region(void);

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Please provide correct arguments");
		return 1;
	}

	FILE *fp;
	fp = fopen("config.txt", "r+");

	fseek(fp, 0, SEEK_END);
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

	enter_region();
	increment(numberOfRowsToAppend, fileName);
	leave_region();

	return 0;
}

void increment(int numberOfRowsToAppend, char *fileName) {
	FILE *fp;
	fp = fopen(fileName, "r+");
	
	int read;
	while(!feof(fp)) {
		fscanf(fp, "%d", &read);
	}

	int i;
	for(i=1; i<=numberOfRowsToAppend; i++) {
		fprintf(fp, "%d\n", read+i);
	}

	fclose(fp);
}

void enter_region()
{
	int status;
	while (get_sv(otherProcess, &status) == 1);
	set_sv(1, &status);
}

void leave_region()
{
	int status;
	set_sv(0, &status);	
}
