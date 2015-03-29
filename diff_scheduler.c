#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct job {
	int len;
	int w;
	int diff;
};

void swap(struct job *a, struct job *b)
{
	struct job tmp;
	memcpy(&tmp, a, sizeof(struct job));
	memcpy(a, b, sizeof(struct job));
	memcpy(b, &tmp, sizeof(struct job));
}

void merge_sort(struct job *a, size_t n)
{
	size_t i, j, k;
	struct job *jobs;

	if (n == 1)
		return;
	k = n / 2;
	merge_sort(a, k);
	merge_sort(a + k, n - k);
	if (!(jobs = malloc(sizeof(struct job) * n))) {
		fprintf(stderr, "Sort error: cannot allocate memory\n");
		exit(1);
	}
	for (i = 0, j = 0; i < n; i++) {
		if (j >= n / 2) {
			memcpy(jobs + i, a + k, sizeof(jobs[i]) * (n - i));
			break;
		}
		if (k >= n) {
			memcpy(jobs + i, a + j, sizeof(jobs[i]) * (n - i));
			break;
		}
		if (a[j].diff > a[k].diff)
			memcpy(jobs + i, a + j++, sizeof(jobs[i]));
		else if (a[j].diff < a[k].diff)
			memcpy(jobs + i, a + k++, sizeof(jobs[i]));
		else if (a[j].w >= a[k].w)
			memcpy(jobs + i, a + j++, sizeof(jobs[i]));
		else
			memcpy(jobs + i, a + k++, sizeof(jobs[i]));

	}
	memcpy(a, jobs, sizeof(a[0]) * n);
	free(jobs);
}

struct job *read_jobs(char *file, size_t *n)
{
	FILE *fp;
	struct job *jobs;
	size_t i;

	/* Open file to read */
	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr, "Open file %s: %s\n", file, strerror(errno));
		exit(1);
	}

	/* Read number of jobs */
	if (fscanf(fp, "%zd", n) == EOF) {
		fprintf(stderr, "Read file %s: %s\n", file, strerror(errno));
		exit(1);
	}

	if (!(jobs = calloc(sizeof(struct job), *n))) {
		fprintf(stderr, "Cannot allocate memory\n");
		exit(1);
	}

	/* Read jobs */
	for (i = 0; i < *n; i++) {
		int len, w;
		if (fscanf(fp, "%d%d", &w, &len) == EOF)
			break;
		jobs[i].w = w;
		jobs[i].len = len;
		jobs[i].diff = w - len;
	}

	return jobs;
}

int main(int argc, char **argv)
{
	struct job *jobs;
	size_t n, i;
	unsigned long res = 0, c = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}

	jobs = read_jobs(argv[1], &n);
	merge_sort(jobs, n);

	for (i = 0; i < n; i++) {
		c += jobs[i].len;
		res += c * jobs[i].w;
	}

	printf("%lu\n", res);

	return 0;
}
