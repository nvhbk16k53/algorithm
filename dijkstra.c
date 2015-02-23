#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#define NELEMS(x) (sizeof(x))/(sizeof((x)[0]))

struct Heap_T {
	size_t v;
	size_t d;
}heap[10000];

struct Vertex_T {
	int stt;
	struct Edge_T {
		size_t v;
		size_t len;
		struct Edge_T *link;
	} *head;
}g[200 + 1];

size_t len = 0, heap_len;

static void swap(struct Heap_T *a, struct Heap_T *b)
{
	struct Heap_T tmp;
	tmp.v = a->v;
	tmp.d = a->d;
	a->v = b->v;
	a->d = b->d;
	b->v = tmp.v;
	b->d = tmp.d;
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

size_t *dijkstra(struct Vertex_T *g, size_t n, size_t s) {
	size_t *a, i, v;
	int *x;
	assert(g);
	assert(n > 0);
	assert(s > 0);
	a = malloc(sizeof(size_t) * n + 1);
	assert(a);
	x = malloc(sizeof(int) * n + 1);
	assert(x);
	for (i = 0; i <= n; i++) {
		a[i] = 1000000;
		x[i] = 0;
	}
	x[s] = 1;
	a[s] = 0;
	v = s;
	heap_len = 0;
	while (1) {
		struct Edge_T *p;
		struct Heap_T *w;
		for (p = g[v].head; p != NULL; p = p->link) {
			size_t d = a[v] + p->len;
			if (x[p->v] == 0)
				Heap_ins(p->v, d);
		}
		do {
			w = Heap_min();
		} while (w != NULL && x[w->v] == 1);
		if (w == NULL)
			break;
		x[w->v] = 1;
		a[w->v] = w->d;
		v = w->v;
	}
	free(x);
	return a;
}

void load(const char *filename)
{
	FILE *fp;
	char buf[BUFSIZ];

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Cannot open file %s\n", filename);
		exit(1);
	}
	while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
		size_t i;
		char *tmp = buf;
		i = 0;
		while (isspace(*tmp))
			tmp++;
		while (isdigit(*tmp)) {
			i = i * 10 - '0' + *tmp;
			tmp++;
		}
		g[i].stt = i;
		if (i > len)
			len = i;
		g[i].head = NULL;
		while (*tmp != '\0') {
			size_t v, len;
			struct Edge_T *e;
			while (isspace(*tmp))
				tmp++;
			v = len = 0;
			while (isdigit(*tmp)) {
				v = v * 10 - '0' + *tmp;
				tmp++;
			}
			if (*(tmp++) != ',')
				return;
			while (isdigit(*tmp)) {
				len = len * 10 - '0' + *tmp;
				tmp++;
			}
			while (isspace(*tmp))
				tmp++;
			e = malloc(sizeof(struct Edge_T));
			e->v = v;
			e->len = len;
			e->link = g[i].head;
			g[i].head = e;
		}
	}
}

int main(int argc, char **argv)
{
	size_t i, *a, s;
	char *tmp;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	load(argv[1]);
	a = dijkstra(g, len, 1);

	for (i = 1; i <= len; i++) {
		printf("%zd\t", a[i]);
		if (i % 10 == 0)
			printf("\n");
	}
	printf("\n");

	exit(0);
}
