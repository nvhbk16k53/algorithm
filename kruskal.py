import sys
import time


def find(u, item):
	root = item;
	while root != u[root]['parent']:
		root = u[root]['parent'];
	while item != u[item]['parent']:
		p = u[item]['parent']
		u[item]['parent'] = root
		item = p
	return root


def union(u, x, y):
	rx = find(u, x)
	ry = find(u, y)
	if u[rx]['rank'] > u[ry]['rank']:
		u[ry]['parent'] = rx
	else:
		if u[rx]['rank'] == u[ry]['rank']:
			u[rx]['rank'] += 1
		u[rx]['parent'] = ry


def prepare(n):
	l = list()
	for i in range(n):
		l.append(1 << i)
	for i in range(n):
		j = i + 1
		while j < n:
			l.append((1 << i) | (1 << j))
			j = j + 1
	return l;


def kruskal(a):
	nclusters = len(a)
	dist = prepare(24)
	u = range(max(a.values()) + 1)
	for i in a:
		u[a[i]] = {'parent': a[i], 'rank': 0}
	for d in dist:
		for i in a:
			j = d ^ i
			if j in a:
				if find(u, a[i]) != find(u, a[j]):
					union(u, a[i], a[j])
					nclusters -= 1;
	return nclusters


def load_data(filename):
	with open(filename) as f:
		d = dict()
		n, m = f.readline().split()
		n = int(n)
		m = int(m)
		count = 0
		for i in range(n):
			tmp = f.readline().split()
			a = 0
			for j in range(m):
				a = (a << 1) | int(tmp[j])
			if a not in d:
				d[a] = count
				count += 1
	return d


if __name__ == '__main__':
	if len(sys.argv) != 3:
		print("usage: %s <file> <space>", sys.argv[0])
		sys.exit(1);
	t = time.time()
	a = load_data(sys.argv[1])
	#import ipdb; ipdb.set_trace()
	print("Number of clusters: ", kruskal(a))
	print("Elapsed time: ", time.time() - t)
