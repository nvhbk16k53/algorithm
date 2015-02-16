#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "stack.h"
#define NVERS 1000000
#define EXPLORED 1

struct Vertex_T {
	int stt;
	struct Edge_T {
		int v;
		struct Edge_T *link;
	} *head;
};

Stack_T stk;

void load(const char *filename, struct Vertex_T **g,
		struct Vertex_T **revg, size_t *n)
{
	FILE *fp;
	size_t i;
	
	/* open file to read */
	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot read from file %s\n", filename);
		exit(1);
	}

	/* Allocate memory for graph */
	*g = malloc(sizeof(struct Vertex_T) * NVERS);
	assert(*g);
	*revg = malloc(sizeof(struct Vertex_T) * NVERS);
	assert(*revg);
	for (i = 0; i < NVERS; i++) {
		(*g)[i].stt = !EXPLORED;
		(*g)[i].head = NULL;
		(*revg)[i].stt = !EXPLORED;
		(*revg)[i].head = NULL;
	}

	/* Build graph and reverse graph */
	*n = 0;
	while (*n < NVERS) {
		int32_t a, b;
		struct Edge_T *e, *reve;
		if (fscanf(fp, "%d%d", &a, &b) != 2)
			break;
		e = malloc(sizeof(struct Edge_T));
		assert(e);
		e->v = b;
		e->link = (*g)[a].head;
		(*g)[a].head = e;
		reve = malloc(sizeof(struct Edge_T));
		assert(reve);
		reve->v = a;
		reve->link = (*revg)[b].head;
		(*revg)[b].head = reve;
		if (*n < a)
			*n = a;
		if (*n < b)
			*n = b;
	}
}

size_t *dfs_loop(struct Vertex_T *g, size_t n)
{
	size_t *order, i, t;

	order = malloc(sizeof(size_t) * (n + 1));
	assert(order);
	t = 0;
	for (i = n; i > 0; i--) {
		struct Vertex_T *v = &g[i];
		struct Edge_T *e = v->head;
		if (v->stt != EXPLORED) {
dfs:
			v->stt = EXPLORED;
			while (e != NULL) {
				if (g[e->v].stt != EXPLORED) {
					Stack_push(stk, v);
					Stack_push(stk, e);
					v = &g[e->v];
					e = v->head;
					goto dfs;
				}
				e = e->link;
			}
			order[++t] = v - g;
			if (!Stack_empty(stk)) {
				e = Stack_pop(stk);
				v = Stack_pop(stk);
				goto dfs;
			}
		}
	}
	return order;
}

size_t *scc(struct Vertex_T *g, struct Vertex_T *revg, size_t n)
{
	size_t *res, i, j, *order;
	res = malloc(sizeof(size_t) * n);
	assert(res);
	bzero(res, sizeof(size_t) * n);

	order = dfs_loop(revg, n);
	for (j = 0, i = n; i > 0; i--) {
		struct Vertex_T *v = &g[order[i]];
		struct Edge_T *e = v->head;
		if (v->stt != EXPLORED) {
dfs:
			v->stt = EXPLORED;
			while (e != NULL) {
				if (g[e->v].stt != EXPLORED) {
					Stack_push(stk, v);
					Stack_push(stk, e);
					v = &g[e->v];
					e = v->head;
					goto dfs;
				}
				e = e->link;
			}
			res[j]++;
			if (!Stack_empty(stk)) {
				e = Stack_pop(stk);
				v = Stack_pop(stk);
				goto dfs;
			}
			j++;
		}
	}
	free(order);
	order = NULL;
	return res;
}

void swap(size_t *a, size_t *b)
{
	size_t tmp = *a;
	*a = *b;
	*b = tmp;
}

void merge_sort(size_t a[], size_t len)
{
	size_t i, j, k;

	if (len <= 1) {
		return;
	} else if (len == 2) {
		if (a[0] > a[1])
			swap(&a[0], &a[1]);
	} else {
		size_t *tmp;
		merge_sort(a, len / 2);
		merge_sort(a + len / 2, len - len / 2);
		tmp = malloc(sizeof(size_t) * len);
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

int main(int argc, char **argv)
{
	struct Vertex_T *g, *revg;
	size_t i, n, *res;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	/* Build graph and reverse graph from file */
	stk = Stack_new();
	load(argv[1], &g, &revg, &n);
	res = scc(g, revg, n);
	merge_sort(res, n);
	for (i = 1; i <= 5; i++)
		printf("%zd\t", res[n - i]);
	printf("\n");

	exit(0);
}
