#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void merge_sort(int a[], int len)
{
	int i, j, k;

	if (len <= 1) {
		return;
	} else if (len == 2) {
		if (a[0] > a[1])
			swap(&a[0], &a[1]);
	} else {
		int *tmp;
		merge_sort(a, len / 2);
		merge_sort(a + len / 2, len - len / 2);
		tmp = malloc(sizeof(int) * len);
		for (i = 0, j = 0, k = len / 2; i < len; i++)
			if (k >= len)
				tmp[i] = a[j++];
			else if (j >= len / 2)
				tmp[i] = a[k++];
			else if (a[j] > a[k])
				tmp[i] = a[k++];
			else
				tmp[i] = a[j++];
		for (i = 0; i < len; i++)
			a[i] = tmp[i];
	}
}

int to_int(const char *str)
{
	int res = 0;
	for (; *str != '\0'; str++)
		res = res * 10 + *str - '0';
	return res;
}

void read_to_array(int a[], char *filename, int *len)
{
	FILE *fp;
	char buf[20];

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Cannot read file %s\n", filename);
		exit(1);
	}
	*len = 0;
	while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
		buf[strlen(buf) - 1] = '\0';
		a[*len] = to_int(buf);
		*len = *len + 1;
	}
}

int main(int argc, char **argv)
{
	int a[100000], len = 0, i;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		return 1;
	}

	read_to_array(a, argv[1], &len);
	merge_sort(a, len);
	return 0;
}
