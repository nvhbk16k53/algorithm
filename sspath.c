#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define FLOYD_WARSHALL 0
#define JOHNSON 1

struct vertex {
	size_t v;
	struct edge *e;
} *vertices;

struct edge {
	size_t u;
	size_t v;
	int cost;
	struct edge *n;
} *edges;

char *progname, **files;
const char *opts = "hc:";
size_t nfile, nver, nedge, alg = JOHNSON;


/* Return edge (u, v) if exist or NULL otherwise */
struct edge *get_edge(struct vertex *g, size_t u, size_t v)
{
	struct edge *e;
	for (e = g[u - 1].e; e != NULL; e = e->n) {
		if (e->v == v)
			return e;
	}
	return NULL;
}

int floyd_warshall(struct vertex *g, size_t n, size_t m)
{
	int *a, *prev_a, min, aij, aik, akj;
	size_t i, j, k;
	struct edge *e;

	assert(g);
	n++;
	a = malloc(sizeof(int) * n * n);
	assert(a);
	prev_a = malloc(sizeof(int) * n * n);
	assert(prev_a);

	/* initialize step */
	for (i = 1; i < n; i++) {
		for (j = 1; j < n; j++) {
			if (i == j)
				/* a[0][i][j] */
				*(prev_a + i * n + j) = 0;
			else if ((e = get_edge(g, i, j)))
				/* a[0][i][j] */
				*(prev_a + i * n + j) = e->cost;
			else
				/* a[0][i][j] */
				*(prev_a + i * n + j) = INT_MAX;
		}
	}

	/* compute shortest path */
	for (k = 1; k < n; k++) {
		for (i = 1; i < n; i++) {
			for (j = 1; j < n; j++) {
				aij = *(prev_a + i * n + j);
				aik = *(prev_a + i * n + k);
				akj = *(prev_a + k * n + j);
				if (aik == INT_MAX || akj == INT_MAX)
					*(a + i * n + j) = aij;
				else
					*(a + i * n + j) = MIN(aij, aik + akj);
			}
		}
		memcpy(prev_a, a, sizeof(int) * n * n);
	}

	/* Get shortest shortest path or NULL if has negative cycle */
	min = INT_MAX;
	for (i = 1; i < n; i++) {
		/* a[n][i][i] < 0 */
		if (*(a + i * n + i) < 0)
			return INT_MAX;
		for (j = 1; j < n; j++) {
			if (i == j) continue;
			min = MIN(min, *(a + i * n + j));
		}
	}
	free(a); a = NULL;
	free(prev_a); prev_a = NULL;
	return min;
}

int load_data(const char *file)
{
	FILE *fp;
	size_t i, u, v;
	int c;

	assert(file);
	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr, "Cannot open file %s: %s",
		        file, strerror(errno));
		return 0;
	}
	if (fscanf(fp, "%zd%zd", &nver, &nedge) == EOF) {
		fprintf(stderr, "Read file %s error: %s\n",
		        file, strerror(errno));
		return 0;
	}

	vertices = malloc(sizeof(struct vertex) * nver);
	assert(vertices);
	edges = malloc(sizeof(struct edge) * nedge);
	assert(edges);

	/* Initialize vertices array */
	for (i = 0; i < nver; i++) {
		vertices[i].v = i + 1;
		vertices[i].e = NULL;
	}

	/* Read data to vertices array and edges array */
	for (i = 0; i < nedge; i++) {
		if (fscanf(fp, "%zd%zd%d", &u, &v, &c) == EOF) {
			fprintf(stderr, "Read file %s error: %s\n",
			        file, strerror(errno));
			return 0;
		}
		if (!u || !v) {
			fprintf(stderr,
			        "Vertex labels must be positive interger\n");
			return 0;
		}
		edges[i].u = u;
		edges[i].v = v;
		edges[i].cost = c;
		edges[i].n = vertices[u - 1].e;
		vertices[u - 1].e = edges + i;
	}
	return 1;
}

void print_help(int err)
{
	FILE *fp;
	if (err)
		fp = stderr;
	else
		fp = stdout;
	fprintf(fp, "Usage:\t%s [-h -c num] data_file...\n", progname);
	fprintf(fp, "Compute shortest shortest path of given graphs\n\n");
	fprintf(fp, "options:\n");
	fprintf(fp, "  -h\t\tdisplay this help and exit\n");
	fprintf(fp, "  -c num\tChoose algorithm. "
	            "0 for Floyd Warshall, 1 for Johnson.\n"
	            "\t\tDefault is Johnson.\n");
	exit(0);
}

void init(int argc, char **argv)
{
	int c, num;
	char *tmp;
	const char *usagefmt = "Usage:\t%s [-h -c num] data_file...\n";

	/* Get standalone program name */
	progname = *argv;
	while ((tmp = strchr(progname,  '/')) != NULL)
		progname = tmp + 1;

	/* Parse options */
	opterr = 0;
	while ((c = getopt(argc, argv, opts)) != -1) {
		switch (c) {
		case 'h':
			print_help(0);
		case 'c':
			num = 0;
			for (tmp = optarg; *tmp != '\0'; tmp++) {
				if (!isdigit(*tmp))
					print_help(1);
				num = num * 10 - '0' + *tmp;
			}
			if (num != FLOYD_WARSHALL && num != JOHNSON)
				print_help(1);
			alg = num;
			break;
		case '?':
			print_help(1);
		default:
			abort();
		}
	}
	if (argc - optind < 1) {
		fprintf(stderr, usagefmt, progname);
		exit(1);
	}
	nfile = argc - optind;
	files = argv + optind;
}

int main(int argc, char **argv)
{
	size_t i;
	int sspath;

	init(argc, argv);

	for (i = 0; i < nfile; i++) {
		if (!load_data(files[i])) {
			fprintf(stderr,
			        "Ignore wrong file format: %s\n", files[i]);
			continue;
		}
		sspath = floyd_warshall(vertices, nver, nedge);
		printf("Shortest shortest path for graph %s: %d\n",
		       files[i], sspath);
		free(vertices); vertices = NULL;
		free(edges); edges = NULL;
	}

	return 0;
}
