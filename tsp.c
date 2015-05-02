#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)


char *progname, *file;
size_t npoint;
double **d;

struct point {
	double x;
	double y;
}*points;

void distance(struct point *p, size_t n)
{
	size_t i, j;
	double a, b, dist;
	assert(p);
	for (i = 0; i < n; i++) {
		for (j = i + 1; j < n; j++) {
			a = p[i].x - p[j].x;
			b = p[i].y - p[j].y;
			dist = sqrt(a * a + b * b);
			d[i][j] = d[j][i] = dist;
		}
	}
}

double total_dist(size_t *route, size_t n)
{
	size_t i;
	double res = 0;
	assert(route);
	for (i = 0; i < n; i++) {
		res += d[route[i]][route[i + 1]];
	}
	return res;
}

int _2opt_swap(size_t *new_route, size_t *route, size_t n, int i, int k)
{
	int j;
	for (j = 0; j < i; j++)
		new_route[j] = route[j];
	for (j = i; j <= k; j++)
		new_route[j] = route[k - j + i];
	for (j = k + 1; j <= n; j++)
		new_route[j] = route[j];
}

double tsp_2opt(struct point *p, size_t n)
{
	size_t i, j, *route, *new_route, root = 0, *best_route;
	double best_dist, new_dist;

	assert(p);
	d = malloc(sizeof(double *) * n); assert(d);
	for (i = 0; i < n; i++) {
		d[i] = malloc(sizeof(double) * n); assert(d[i]);
	}
	distance(p, n);
	new_route = malloc(sizeof(size_t) * (n + 1)); assert(new_route);
	best_route = malloc(sizeof(size_t) * (n + 1)); assert(best_route);
again:
	route = malloc(sizeof(size_t) * (n + 1)); assert(route);
	for (i = 0; i <= n; i++)
		route[i] = (i + root) % n;
loop:
	best_dist = total_dist(route, n);
	for (i = 1; i < n - 1; i++) {
		for (j = i + 1; j < n; j++) {
			_2opt_swap(new_route, route, n, i, j);
			new_dist = total_dist(new_route, n);
			if (new_dist < best_dist) {
				memcpy(route, new_route, sizeof(size_t) * (n + 1));
				goto loop;
			}
		}
	}
	if (root == 0 || best_dist < total_dist(best_route, n))
		memcpy(best_route, route, sizeof(size_t) * (n + 1));
	root += 1;
	if (root < n)
		goto again;
	return total_dist(best_route, n);
}

int load_data(const char *file)
{
	FILE *fp;
	size_t i;
	double x, y;

	assert(file);
	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr,
		        "Open file %s error: %s", file, strerror(errno));
		exit(1);
	}
	if (fscanf(fp, "%zd", &npoint) == EOF) {
		fprintf(stderr,
		        "Read file %s error: %s", file, strerror(errno));
		exit(1);
	}
	points = malloc(sizeof(struct point) * npoint);
	assert(points);
	for (i = 0; i < npoint; i++) {
		if (fscanf(fp, "%lf%lf", &x, &y) == EOF) {
			fprintf(stderr,
			        "Read file %s error: %s",
				file, strerror(errno));
			exit(1);
		}
		points[i].x = x;
		points[i].y = y;
	}
	fclose(fp);
	return 1;
}

void print_help()
{
	fprintf(stderr, "Usage:\t%s [-h] file\n", progname);
	fprintf(stderr, "Compute shortest shortest path of given graphs\n\n");
	fprintf(stderr, "options:\n");
	fprintf(stderr, "  -h\t\tdisplay this help and exit\n");
	exit(0);
}

void init(int argc, char **argv)
{
	int c, num;
	char *tmp;
	const char *usagefmt = "Usage:\t%s [-h] file\n";

	/* Get standalone program name */
	progname = *argv;
	while ((tmp = strchr(progname,  '/')) != NULL)
		progname = tmp + 1;

	/* Parse options */
	opterr = 0;
	while ((c = getopt(argc, argv, "h")) != -1) {
		switch (c) {
		case 'h':
			print_help();
		case '?':
			print_help();
		default:
			abort();
		}
	}
	if (argc - optind < 1) {
		fprintf(stderr, usagefmt, progname);
		exit(1);
	}
	file = argv[optind];
}

int main(int argc, char **argv)
{
	size_t i;

	init(argc, argv);
	load_data(file);

	printf("TSP Solution: %lf\n", tsp_2opt(points, npoint));

	return 0;
}
