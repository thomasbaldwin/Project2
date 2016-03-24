#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s m n filename\n", argv[0]);
		return 1;
	}

	int numberOfRowsToAppend = atoi(argv[1]);
	char *fileName = argv[2];

	FILE *fp;
	int read;

	fp = fopen(fileName, "r");
	while(!feof(fp)) {
		fscanf(fp, "%d", &read);
	}
	fclose(fp);

	int i=0;
	for(i=0; i<numberOfRowsToAppend; i++) {
		fp = fopen(fileName, "a+");
		fprintf(fp, "%d\n", read+i);
		fclose(fp);
	}

	return 0;
}
