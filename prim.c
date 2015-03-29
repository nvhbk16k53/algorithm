#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define NELEMS(x) (sizeof(x))/(sizeof((x)[0]))

struct Heap_T {
	int v;
	int d;
}heap[100000];

struct Vertex_T {
	int label;
	struct Edge_T {
		int v;
		int len;
		struct Edge_T *link;
	} *head;
}*g;

struct mst {
	int v;
	int w;
	int c;
};

int nnodes, nedges, heap_len;

static void swap(struct Heap_T *a, struct Heap_T *b)
{
	struct Heap_T tmp;
	memcpy(&tmp, a, sizeof(tmp));
	memcpy(a, b, sizeof(*a));
	memcpy(b, &tmp, sizeof(*b));
}

void Heap_ins(size_t v, size_t d)
{
	size_t i, j;
	assert(heap_len < NELEMS(heap));
	heap[heap_len].v = v;
	heap[heap_len].d = d;
	heap_len++;
	for (i = heap_len; i > 1; i = j) {
		j = i / 2;
		if (heap[i - 1].d < heap[j - 1].d)
			swap(&heap[i - 1], &heap[j - 1]);
		else
			break;
	}
}

struct Heap_T *Heap_min(void)
{
	size_t i, j;
	if (heap_len == 0) {
		return NULL;
	} else if (heap_len == 1) {
		return &heap[--heap_len];
	} else {
		swap(&heap[0], &heap[--heap_len]);
		for (i = 1; i <= heap_len / 2; i = j) {
			j = 2 * i;
			if (j + 1 < heap_len && heap[j - 1].d > heap[j].d)
				j = j + 1;
			if (heap[i - 1].d > heap[j - 1].d)
				swap(&heap[i - 1], &heap[j - 1]);
			else
				break;
		}
		return &heap[heap_len];
	}
}

struct mst *prim(struct Vertex_T *g, size_t n, size_t s) {
	size_t i, v;
	int *x;
	struct mst *t;

	assert(g);
	assert(n > 0);
	assert(s > 0);

	x = calloc(sizeof(int), n + 1);
	assert(x);
	t = calloc(sizeof(struct mst), n);
	assert(t);

	x[s] = 1;
	v = s;
	heap_len = 0;
	i = 0;
	while (1) {
		struct Edge_T *p;
		struct Heap_T *w;
		for (p = g[v].head; p != NULL; p = p->link) {
			if (x[p->v] == 0)
				Heap_ins(p->v, p->len);
		}
		do {
			w = Heap_min();
		} while (w != NULL && x[w->v] == 1);
		if (w == NULL)
			break;
		x[w->v] = 1;
		t[i].v = v;
		t[i].w = w->v;
		t[i].c = w->d;
		v = w->v;
		i++;
	}
	free(x);
	return t;
}

void load(const char *filename)
{
	FILE *fp;
	char buf[BUFSIZ];
	size_t i;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Cannot open file %s\n", filename);
		exit(1);
	}
	/* Read number of nodes and number of edges */
	if(fscanf(fp, "%d%d", &nnodes, &nedges) == EOF) {
		fprintf(stderr, "Read file %s: %s\n", filename, strerror(errno));
		exit(1);
	}
	g = malloc(sizeof(struct Vertex_T) * (nnodes + 1));
	assert(g);
	for (i = 0; i < nedges; i++) {
		struct Edge_T *e;
		int v, w, c;
		if (fscanf(fp, "%d%d%d", &v, &w, &c) == EOF)
			break;
		e = malloc(sizeof(struct Edge_T) * 2);
		assert(e);
		e[0].v = v;
		e[0].len = c;
		e[0].link = g[w].head;
		g[w].head = e;
		e[1].v = w;
		e[1].len = c;
		e[1].link = g[v].head;
		g[v].head = e + 1;
	}
	fclose(fp);
}

int main(int argc, char **argv)
{
	size_t i, *a, s;
	char *tmp;
	struct mst *t;
	long cost = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	load(argv[1]);
	t = prim(g, nnodes, 1);

	for (i = 0; i < nnodes; i++) {
		printf("%d\t", t[i].c);
		cost += t[i].c;
	}
	printf("\n%ld\n", cost);

	exit(0);
}
