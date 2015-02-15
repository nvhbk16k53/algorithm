#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

int quicksort(int a[], int first, int last)
{
	if (last <= first + 1) {
		return 0;
	} else {
		int i, j, ncomps, pivot;
		pivot = a[first];
		for (i = j = first + 1; j < last; j++) {
			if (a[j] < pivot)
				swap(&a[i++], &a[j]);
		}
		swap(&a[i - 1], &a[first]);
		return last - first - 1 + quicksort(a, first, i - 1) + quicksort(a, i, last);
	}
}

int quicksort2(int a[], int first, int last)
{
	if (last <= first + 1) {
		return 0;
	} else {
		int i, j, ncomps, pivot;
		swap(&a[first], &a[last - 1]);
		pivot = a[first];
		for (i = j = first + 1; j < last; j++) {
			if (a[j] < pivot)
				swap(&a[i++], &a[j]);
		}
		swap(&a[i - 1], &a[first]);
		return last - first - 1 + quicksort2(a, first, i - 1) + quicksort2(a, i, last);
	}
}

int median(int a[], int first, int last)
{
	int m;
	m = (last - first - 1) / 2 + first;
	if ((a[m] < a[first]) && (a[first] < a[last - 1]) || (a[m] > a[first] && a[first] > a[last - 1]))
		return first;
	else if ((a[m] > a[last - 1]) && (a[first] < a[last - 1]) || (a[m] < a[last - 1] && a[first] > a[last - 1]))
		return last - 1;
	else
		return m;
}

int quicksort3(int a[], int first, int last)
{
	if (last <= first + 1) {
		return 0;
	} else {
		int i, j, ncomps, pivot;
		i = median(a, first, last);
		swap(&a[i], &a[first]);
		pivot = a[first];
		for (i = j = first + 1; j < last; j++) {
			if (a[j] < pivot)
				swap(&a[i++], &a[j]);
		}
		swap(&a[i - 1], &a[first]);
		return last - first - 1 + quicksort3(a, first, i - 1) + quicksort3(a, i, last);
	}
}

int load(const char *filename, int a[], int len)
{
	FILE *fp;
	int i;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Could not open file %s\n", filename);
		exit(1);
	}

	for (i = 0; i < len; i++)
		if (fscanf(fp, "%d", &a[i]) == EOF)
			break;
	return i;
}

int main(int argc, char **argv)
{
	int i, a[10000], len;

	srand(time(NULL));

	if (argc != 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}

	len = load(argv[1], a, sizeof(a));
	printf("%d\n", quicksort(a, 0, len));
	len = load(argv[1], a, sizeof(a));
	printf("%d\n", quicksort2(a, 0, len));
	len = load(argv[1], a, sizeof(a));
	printf("%d\n", quicksort3(a, 0, len));

	/*for (i = 0; i < len; i++)
		printf("%d\n", a[i]);*/

	return 0;
}
