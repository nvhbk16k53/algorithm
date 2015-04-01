#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

struct graph {
	int *v;
	struct edge {
		int u;
		int v;
		int d;
		struct edge *next;
	}*e;
	size_t n, m;
};

struct heap {
	void *elem;
	int k;
};

struct union_find {
	size_t parent;
	size_t rank;
};

void swap(struct heap *a, struct heap *b)
{
	struct heap tmp;
	memcpy(&tmp, a, sizeof(tmp));
	memcpy(a, b, sizeof(*a));
	memcpy(b, &tmp, sizeof(*b));
}

void *heap_ext(struct heap *heap, size_t *n)
{
	size_t i, j;

	assert(heap);
	if (*n == 0)
		return NULL;
	else if (*n == 1)
		return heap[--(*n)].elem;
	swap(&heap[0], &heap[--(*n)]);
	for (i = 1; i <= *n / 2; i = j) {
		j = 2 * i;
		if ((j + 1 < *n) && (heap[j - 1].k > heap[j].k))
			j++;
		if (heap[i - 1].k < heap[j - 1].k)
			break;
		else
			swap(&heap[i - 1], &heap[j - 1]);
	}

	return heap[*n].elem;
}

void heap_ins(struct heap *heap, size_t *n, struct heap x)
{
	size_t i, j;
	assert(heap);
	memcpy(&heap[(*n)++], &x, sizeof(*heap));
	for (i = *n - 1; i > 0; i = j) {
		j = (i + 1) / 2 - 1;
		if (heap[i].k < heap[j].k)
			swap(&heap[i], &heap[j]);
		else
			break;
	}
}

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

int kruskal(struct graph g, size_t k)
{
	struct edge *e;
	struct heap *heap;
	size_t hlen = 0, i, nclusters;
	struct union_find *u;

	assert(k >= 0);
	assert(g.v && g.e);

	nclusters = g.n;
	u = malloc(sizeof(struct union_find) * (g.n + 1));
	assert(u);
	for (i = 0; i <= nclusters; i++) {
		u[i].rank = 0;
		u[i].parent = i;
	}
	heap = malloc(sizeof(struct heap) * g.m);
	assert(heap);
	for (e = g.e; e != NULL; e = e->next) {
		struct heap x;
		x.elem = e;
		x.k = e->d;
		heap_ins(heap, &hlen, x);
	}
	
	while (nclusters > k) {
		if (!(e = heap_ext(heap, &hlen)))
			return -1;
		if (find(u, e->u) != find(u, e->v)) {
			_union(u, e->u, e->v);
			nclusters--;
		}
	}

	e = heap_ext(heap, &hlen);
	while ((e = heap_ext(heap, &hlen))) {
		if (find(u, e->u) != find(u, e->v))
			return e->d;
	}
	return -1;
}

struct graph load_graph(char *file)
{
	FILE *fp;
	struct graph g;
	int u, v, d;

	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr, "load_graph: open file error: %s\n",
		        strerror(errno));
		exit(1);
	}

	if (fscanf(fp, "%zd", &g.n) == EOF) {
		fprintf(stderr, "load_graph: read file error: %s\n",
		        strerror(errno));
		exit(1);
	}
	g.v = malloc(sizeof(int) * (g.n + 1));
	g.e = NULL;
	g.m = 0;
	assert(g.v);
	while (fscanf(fp, "%d%d%d", &u, &v, &d) != EOF)
	{
		struct edge *e;
		g.v[u] = u;
		g.v[v] = v;
		e = malloc(sizeof(struct edge));
		assert(e);
		e->u = u;
		e->v = v;
		e->d = d;
		e->next = g.e;
		g.e = e;
		g.m++;
	}

	return g;
}

int to_int(const char *num)
{
	size_t res = 0;
	while (*num != '\0') {
		if (!isdigit(*num))
			return -1;
		res = *(num++) - '0' + res * 10;
	}
	return res;
}

int main(int argc, char **argv)
{
	struct graph g;
	struct edge *e;
	int k;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <file> <cluster_number>\n", argv[0]);
		exit(1);
	}
	if ((k = to_int(argv[2])) <= 0) {
		fprintf(stderr, "number of clusters must be positive\n");
		exit(1);
	}
	g = load_graph(argv[1]);
	printf("%d\n", kruskal(g, k));

	exit(0);
}
