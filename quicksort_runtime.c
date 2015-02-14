#include <stdio.h>
#include <stdlib.h>

int load_input(const char *filename, int a[], int len)
{
	FILE *fp;
	int i;
	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Could not read file %s\n", filename);
		return -1;
	}
	for (i = 0; i < len; i++)
		if (fscanf(fp, "%d", &a[i]) == EOF)
			break;
	return i;
}

int main(int argc, char **argv)
{
	int a[100000], len;
	int i;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}
	len = load_input(argv[1], a, sizeof(a));
	if (len < 0)
		return 1;

	return 0;
}
