#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>

struct graph {
	uint32_t *a;
	size_t n;
	size_t m;
};

struct union_find {
	size_t parent;
	size_t rank;
};

size_t find(struct union_find *u, size_t x)
{
	size_t root;
	for (root = x; root != u[root].parent; root = u[root].parent);
	while (x != root) {
		size_t parent = u[x].parent;
		u[x].parent = root;
		x = parent;
	}
	return root;
}

void _union(struct union_find *u, size_t x, size_t y)
{
	x = find(u, x);
	y = find(u, y);
	if (u[x].rank > u[y].rank) {
		u[x].rank += u[y].rank;
		u[y].parent = x;
	} else {
		if (u[x].rank == u[y].rank)
			u[y].rank++;
		u[x].parent = y;
	}
}

size_t dist(uint32_t a, uint32_t b)
{
	size_t i;
	a = a ^ b;
	b = 0;
	for (i = 0; i < 32; i++) {
		b += a & 1;
		a >>= 1;
	}
	return b;
}

int kruskal(struct graph g, size_t d)
{
	int i, j;
	size_t n = g.n;
	struct union_find *x = malloc(sizeof(struct union_find) * g.n);
	assert(x);
	for (i = 0; i < g.n; i++) {
		x[i].rank = 0;
		x[i].parent = i;
	}

	for (i = 0; i < g.n; i++) {
		for (j = i + 1; j < g.n; j++) {
			if (dist(g.a[i], g.a[j]) <= d) {
				if (find(x, i) != find(x, j)) {
					_union(x, i, j);
					n--;
				}
			}
		}
	}
	return n;
}

void load_data(const char *file, struct graph *g)
{
	FILE *fp;
	size_t i;

	assert(g);
	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr, "load_data: open file %s: %s\n",
		        file, strerror(errno));
		exit(1);
	}
	if (fscanf(fp, "%zd%zd", &g->n, &g->m) == EOF) {
		fprintf(stderr, "load_data: read file %s: %s\n",
		        file, strerror(errno));
		exit(1);
	}
	g->a = malloc(sizeof(uint32_t) * g->n);
	assert(g->a);
	for (i = 0; i < g->n; i++) {
		uint32_t tmp = 0;
		size_t j;
		for (j = 0; j < g->m; j++) {
			int b;
			if (fscanf(fp, "%d", &b) == EOF) {
				fprintf(stderr, "Wrong file format\n");
				exit(1);
			}
			if (b > 1 || b < 0) {
				fprintf(stderr, "Wrong file formet\n");
				exit(1);
			}
			tmp = (tmp << 1) | b;
		}
		g->a[i] = tmp;
	}
}

int to_int(const char *num)
{
	int res = 0, s = 1;
	assert(num);
	if (*num == '+') {
		num++;
	} else if (*num == '-') {
		s = -1;
		num++;
	}
	while (*num != '\0') {
		assert(isdigit(*num));
		res = *(num++) - '0' + res * 10;
	}
	return res * s;
}

int main(int argc, char **argv)
{
	struct graph g;
	int d;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <file> <space>\n", argv[0]);
		exit(1);
	}
	if ((d = to_int(argv[2])) <= 0) {
		fprintf(stderr, "number space must be positive\n");
		exit(1);
	}
	load_data(argv[1], &g);
	kruskal(g, d);

	exit(0);
}
