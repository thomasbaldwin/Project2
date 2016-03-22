#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *fileName = "foo.txt";

	FILE *fp;
	fp = fopen("foo.txt", "r+");

	int currentValue;
	int previousValue;
	
	int index = 0;
	while(fscanf(fp, "%d", &currentValue) == 1) {
		if (index != 0 && currentValue != previousValue + 1) {
			fprintf(stdout, "%d\n", previousValue);
			fprintf(stdout, "%d\n", currentValue);
		}

		previousValue = currentValue;
		index++;
	}

	fclose(fp);
	return 0;
}
