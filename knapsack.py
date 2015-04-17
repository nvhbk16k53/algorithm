import sys
sys.setrecursionlimit(2100)


def load_data(filename):
	global knapsack_size, nitems, items
	with open(filename) as f:
		knapsack_size, nitems = f.readline().strip().split()
		knapsack_size = int(knapsack_size)
		nitems = int(nitems)
		items = list()
		for i in range(nitems):
			v, w = f.readline().strip().split()
			items.append({'v': int(v), 'w': int(w)})


def knapsack(items, n, w):
	global cache, runtime
	runtime += 1
	if n < 0 or w < 0:
		raise Exception("Invalid Argument")
	elif n == 0 or w == 0:
		return 0;
	elif w in cache[n - 1]:
		return cache[n - 1][w]
	elif w < items[n - 1]['w']:
		cache[n - 1][w] = knapsack(items, n - 1, w)
	else:
		cache[n - 1][w]  = max(knapsack(items, n - 1, w), knapsack(items, n - 1, w - items[n - 1]['w']) + items[n - 1]['v'])
	return cache[n - 1][w]


if __name__ == '__main__':
	if (len(sys.argv) != 2):
		print "usage: %s <file>" % sys.argv[0]
		sys.exit(1)
	global runtime
	global knapsack_size, nitems, items, cache
	load_data(sys.argv[1])
	cache = list()
	for i in range(nitems):
		cache.append(dict())
	runtime = 0
	s = knapsack(items, nitems, knapsack_size)
	#print knapsack_size, nitems, items
	print "Running time: %d" % runtime
	print "Solution: %d" % s
	sys.exit(0)
