#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)

struct item {
	int value;
	int weight;
};

struct cache {
	int pos;
	int s;
}*cache;

const char *progname, *file;
int sort = 0;
struct item *items;
size_t knapsack_size, nitems;

void init(int argc, char **argv)
{
	int c;
	char *tmp;

	progname = *argv;
	while ((tmp = strchr(progname,  '/')) != NULL)
		progname = tmp + 1;
	/*argc--; argv++;*/

	opterr = 0;
	while ((c = getopt(argc, argv, "s")) != -1) {
		switch (c) {
		case 's':
			sort = 1;
			break;
		case '?':
			fprintf(stderr, "usage: %s option <file>\n", progname);
			exit(1);
		default:
			abort();
		}
	}
	if (argc - optind != 1) {
		fprintf(stderr, "usage: %s option <file>\n", progname);
		exit(1);
	}
	file = argv[optind];
}

void load_data()
{
	FILE *fp;
	int i, v, w;

	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr, "Open file %s: %s\n", file, strerror(errno));
		exit(1);
	}
	if (fscanf(fp, "%zd%zd", &knapsack_size, &nitems) == EOF) {
		fprintf(stderr, "%s: wrong file format\n", file);
		exit(1);
	}
	if (!(items = malloc(sizeof(struct item) * nitems))) {
		fprintf(stderr, "Cannot allocate memory\n");
		exit(1);
	}
	for (i = 0; i < nitems; i++) {
		if (fscanf(fp, "%d%d", &v, &w) == EOF) {
			fprintf(stderr, "Read %d of %zd records\n", i, nitems);
			exit(1);
		}
		items[i].value = v;
		items[i].weight = w;
	}
}

int knapsack(struct item *a, int n, int w)
{
	assert(n >= 0 && w >= 0);
	assert(a);
	if (n == 0 || w == 0) {
		return 0;
	} else if (cache[w].pos == n) {
		return cache[w].s;
	} else {
		if (a[n - 1].weight > w) {
			cache[w].s = knapsack(a, n - 1, w);
		} else {
			int w1, w2;
			w1 = knapsack(a, n - 1, w);
			w2 = knapsack(a, n - 1, w - a[n - 1].weight);
			cache[w].s = MAX(w1, w2 + a[n - 1].value);
		}
		cache[w].pos = n;
		return cache[w].s;
	}
}

int main(int argc, char **argv)
{
	int s;

	init(argc, argv);
	load_data();
	if (!(cache = calloc(sizeof(struct cache), knapsack_size + 1))) {
		fprintf(stderr, "Cannot allocate cache memory\n");
		exit(1);
	}
	s = knapsack(items, nitems, knapsack_size);
	printf("%d\n", s);

	return 0;
}
