#!/usr/bin/env python
import sys
import os

def load(filename):
    a = set()
    with open(filename) as f:
        s = f.readline()
        while s:
            x = int(s)
            a.add(x)
            s = f.readline()
    return a


def build_hash_table(a):
    d = dict()
    m = min(a)
    for x in a:
        k = (x - m) / 40000
        if k not in d:
            d[k] = set()
        d[k].add(x)
    return d


def two_sum(a):
    d = build_hash_table(a)
    s = [0] * 20001
    m = min(a)
    for x in a:
        y = (-x - m) / 40000
        try:
            l = d[y]
        except KeyError:
            pass
        else:
            for y in l:
                if (x + y >= -10000) and (x + y <= 10000) and x != y:
                    s[x + y + 10000] = 1

    return sum(s)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print "usage: %s <file>" % sys.argv[0]
        sys.exit(1)
    #import ipdb; ipdb.set_trace()
    a = load(sys.argv[1])
    res = two_sum(a)
    print res
    #print a
