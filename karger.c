#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define NROWS 200
#define NCOLS 100
#define NELEMS(x) ((sizeof(x))/(sizeof(x[0])))

struct edge {
	int x;
	int y;
}e[NROWS * NCOLS];

struct vertex {
	int x;
	struct vertex *link;
}v[NROWS + 1];

int a[NROWS][NCOLS];
size_t n, m;

void shuffle(size_t *a, size_t n)
{
	if (n > 1) {
		size_t i;
		srand(time(NULL));
		for (i = 0; i < n - 1; i++) {
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			size_t t = a[j];
			a[j] = a[i];
			a[i] = t;
		}
	}
}

void prepare(const char *file)
{
	FILE *fp;
	size_t i, j;
	char buf[BUFSIZ];

	if ((fp = fopen(file, "r")) == NULL) {
		fprintf(stderr, "Could not open file %s\n", file);
		exit(1);
	}
	for (i = 0; i < NROWS; i++) {
		char *tmp;
		int num;
		if (fgets(buf, sizeof(buf) - 1, fp) == NULL)
			break;
		tmp = buf;
		j = 0;
		while (*tmp != '\0' && j < NCOLS) {
			while (isspace(*tmp))
				tmp++;
			num = 0;
			while (isdigit(*tmp)) {
				num = num * 10 - '0' + *tmp;
				tmp++;
			}
			a[i][j] = num;
			j++;
			while (isspace(*tmp))
				tmp++;
		}
	}
	n = i;
}

void graph(void)
{
	size_t i, j;
	int tmp;
	for (i = 0; i < NELEMS(v); i++) {
		v[i].x = 0;
		v[i].link = &v[i];
	}
	m = 0;
	for (i = 0; i < n; i++) {
		tmp = a[i][0];
		v[tmp].x = tmp;
		for (j = 1; a[i][j] > 0; j++) {
			if (!v[a[i][j]].x) {
				e[m].x = tmp;
				e[m].y = a[i][j];
				m++;
			}
		}
	}
}

int is_valid_edge(size_t i)
{
	return (v[e[i].x].x != v[e[i].y].x);
}

void merge(size_t i)
{
	int x = v[e[i].x].x;
	struct vertex *p = v[e[i].y].link;
	while (p->x != x) {
		p->x = x;
		p = p->link;
	}
	p = v[e[i].x].link;
	v[e[i].x].link = v[e[i].y].link;
	v[e[i].y].link = p;
}

size_t karger(size_t *p, size_t len)
{
	size_t i, j;
	for (i = 1; i <= n; i++) {
		v[i].x = i;
		v[i].link = v + i;
	}
	i = n;
	j = 0;
	while (i > 2) {
		if (!is_valid_edge(p[j])) {
			j++;
			continue;
		}
		merge(p[j++]);
		i--;
	}
	for (i = 0; j < len; j++)
		if (is_valid_edge(p[j]))
			i++;
	return i;
}

size_t karger_min_cut(void)
{
	size_t *perms, i, min = 0, k;
	perms = malloc(sizeof(size_t) * m);
	if (perms == NULL) {
		fprintf(stderr, "karger_min_cut: cannot allocate memory\n");
		exit(1);
	}
	for (i = 0; i < m; i++)
		perms[i] = i;
	for (i = 0; i < n * n * 8; i++) {
		shuffle(perms, m);
		k = karger(perms, m);
		if (min == 0 || min > k)
			min = k;
	}

	free(perms);
	return min;
}

int main(int argc, char **argv)
{
	size_t i, j;
	time_t t1, t2;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	prepare(argv[1]);
	graph();
	t1 = time(NULL);
	printf("Karger min cut: %zd\n", karger_min_cut());
	t2 = time(NULL);
	printf("Running time: %ds\n", (int)(t2 - t1));

	exit(0);
}
