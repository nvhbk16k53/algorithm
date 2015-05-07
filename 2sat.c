#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "stack.h"
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#define EXPLORED 1

struct Vertex_T {
	struct Edge_T {
		int v;
		struct Edge_T *link;
	} *head;
} *g, *revg;

struct clause {
	int x;
	int y;
} *clauses;

Stack_T stk;
size_t n, m;

void load_data(const char *file)
{
	FILE *fp;
	size_t i;
	int x, y;
	
	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr,
		        "Open file %s error: %s\n", file, strerror(errno));
		exit(1);
	}
	if (fscanf(fp, "%zd", &n) == EOF) {
		fprintf(stderr, "Wrong file format\n");
		exit(1);
	}
	m = n;
	clauses = malloc(sizeof(struct clause) * m);
	for (i = 0; i < m; i++) {
		if (fscanf(fp, "%d%d", &x, &y) == EOF) {
			fprintf(stderr, "Wrong file format\n");
			exit(1);
		}
		clauses[i].x = x;
		clauses[i].y = y;
	}
}

void build_graph(struct clause *cls, size_t n, size_t m)
{
	/* Allocate memory for graph */
	size_t i;
	struct Edge_T *e, *reve;

	g = malloc(sizeof(struct Vertex_T) * 2 * (n + 1) + 1); assert(g);
	revg = malloc(sizeof(struct Vertex_T) * 2 * (n + 1) + 1); assert(revg);
	for (i = 0; i < 2 * (n + 1); i++) {
		g[i].head = NULL;
		revg[i].head = NULL;
	}

	/* Build graph and reverse graph */
	for(i = 0; i < n; i++) {
		e = malloc(sizeof(struct Edge_T)); assert(e);
		reve = malloc(sizeof(struct Edge_T)); assert(reve);
		if (cls[i].x < 0) {
			if (cls[i].y < 0) {
				e->v = -cls[i].y * 2 + 1;
				reve->link = revg[-cls[i].y * 2 + 1].head;
				revg[-cls[i].y * 2 + 1].head = reve;
			} else {
				e->v = cls[i].y * 2;
				reve->link = revg[cls[i].y * 2].head;
				revg[cls[i].y * 2].head = reve;
			}
			e->link = g[-cls[i].x * 2].head;
			g[-cls[i].x * 2].head = e;
			reve->v = -cls[i].x * 2;
		} else {
			if (cls[i].y < 0) {
				e->v = -cls[i].y * 2 + 1;
				reve->link = revg[-cls[i].y * 2 + 1].head;
				revg[-cls[i].y * 2 + 1].head = reve;
			} else {
				e->v = cls[i].y * 2;
				reve->link = revg[cls[i].y * 2].head;
				revg[cls[i].y * 2].head = reve;
			}
			e->link = g[cls[i].x * 2 + 1].head;
			g[cls[i].x * 2 + 1].head = e;
			reve->v = cls[i].x * 2 + 1;
		}
		e = malloc(sizeof(struct Edge_T)); assert(e);
		reve = malloc(sizeof(struct Edge_T)); assert(reve);
		if (cls[i].y < 0) {
			if (cls[i].x < 0) {
				e->v = -cls[i].x * 2 + 1;
				reve->link = revg[-cls[i].x * 2 + 1].head;
				revg[-cls[i].x * 2 + 1].head = reve;
			} else {
				e->v = cls[i].x * 2;
				reve->link = revg[cls[i].x * 2].head;
				revg[cls[i].x * 2].head = reve;
			}
			e->link = g[-cls[i].y * 2].head;
			g[-cls[i].y * 2].head = e;
			reve->v = -cls[i].y * 2;
		} else {
			if (cls[i].x < 0) {
				e->v = -cls[i].x * 2 + 1;
				reve->link = revg[-cls[i].x * 2 + 1].head;
				revg[-cls[i].x * 2 + 1].head = reve;
			} else {
				e->v = cls[i].x * 2;
				reve->link = revg[cls[i].x * 2].head;
				revg[cls[i].x * 2].head = reve;
			}
			e->link = g[cls[i].y * 2 + 1].head;
			g[cls[i].y * 2 + 1].head = e;
			reve->v = cls[i].y * 2 + 1;
		}
	}
}

size_t *dfs_loop(struct Vertex_T *g, size_t n)
{
	size_t *order, i, t;
	uint8_t *stt;

	order = malloc(sizeof(size_t) * (n + 1)); assert(order);
	stt = malloc(sizeof(uint8_t) * (n + 1)); assert(stt);
	for (t = 0, i = n; i > 0; i--) {
		struct Vertex_T *v = &g[i];
		struct Edge_T *e = v->head;
		if (stt[i] != EXPLORED) {
dfs1:
			stt[v - g] = EXPLORED;
			while (e != NULL) {
				if (stt[e->v] != EXPLORED) {
					Stack_push(stk, v);
					Stack_push(stk, e);
					v = &g[e->v];
					e = v->head;
					goto dfs1;
				}
				e = e->link;
			}
			order[++t] = v - g;
			if (!Stack_empty(stk)) {
				e = Stack_pop(stk);
				v = Stack_pop(stk);
				goto dfs1;
			}
		}
	}
	free(stt); stt = NULL;
	return order;
}

size_t *scc(struct Vertex_T *g, struct Vertex_T *revg, size_t n)
{
	size_t *res, i, j, *order;
	uint8_t *stt;
	res = calloc(sizeof(size_t), n + 1); assert(res);
	stt = calloc(sizeof(uint8_t), n + 1); assert(stt);

	order = dfs_loop(revg, n);
	for (i = n; i > 0; i--) {
		struct Vertex_T *v = &g[order[i]];
		struct Edge_T *e = v->head;
		if (stt[order[i]] != EXPLORED) {
dfs2:
			stt[v - g] = EXPLORED;
			while (e != NULL) {
				if (stt[e->v] != EXPLORED) {
					Stack_push(stk, v);
					Stack_push(stk, e);
					v = &g[e->v];
					e = v->head;
					goto dfs2;
				}
				e = e->link;
			}
			res[v - g] = order[i];
			if (!Stack_empty(stk)) {
				e = Stack_pop(stk);
				v = Stack_pop(stk);
				goto dfs2;
			}
		}
	}
	free(order); order = NULL;
	free(stt); stt = NULL;

	return res;
}

int _2sat(struct clause *cls, size_t n, size_t m)
{
	size_t *a, i;

	/* Initialize */
	stk = Stack_new(); assert(stk);
	build_graph(cls, n, m);
	a = scc(g, revg, 2 * (n + 1));
	for (i = 1; i < n; i++) {
		if (a[i * 2] == a[i * 2 + 1])
			return 0;
	}
	return 1;
}

int main(int argc, char **argv)
{
	int t;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}

	load_data(argv[1]);
	t = time(NULL);
	printf("Answer: %s\n",
	       _2sat(clauses, n, m) ? "satifisable" : "unsatisfisable");
	printf("Elapsed time: %d\n", (int)time(NULL) - t);

	exit(0);
}
