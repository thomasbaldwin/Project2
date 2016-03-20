#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int status;
	get_sv(40, &status);

	if (argc != 3) {
		printf("Usage: %s m n filename\n", argv[0]);
		return 1;
	}

	int numberOfRowsToAppend = atoi(argv[1]);
	char *fileName = argv[2];

	FILE *fp;
	fp = fopen(fileName, "r");
	int read;

	while(!feof(fp)) {
		fscanf(fp, "%d", &read);
	}

	fp = fopen(fileName, "a");

	int i;
	for(i=1; i<=numberOfRowsToAppend; i++) {
		fprintf(fp, "%d\n", read+i);
	}

	fclose(fp);
	return 0;
}
