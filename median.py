#!/usr/bin/env python

import sys
import os


def load(filename):
    res = list()
    with open(filename) as f:
        s = f.readline()
        while s:
            res.append(int(s))
            s = f.readline()
    return res


def median(a):
    h = set()
    l = set()
    s = 0
    for x in a:
        if len(l) > len(h):
            tmp = 0
            if len(l) > 0:
                tmp = max(l)
            if x < tmp:
                l.remove(tmp)
                l.add(x)
                x = tmp
            h.add(x)
        else:
            tmp = 10001
            if len(h) > 0:
                tmp = min(h)
            if x > tmp:
                h.remove(tmp)
                h.add(x)
                x = tmp
            l.add(x)
        s += max(l)
    return s


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print "usage: %s <file>" % sys.argv[0]
    a = load(sys.argv[1])
    #import ipdb; ipdb.set_trace()
    print median(a) % 10000
