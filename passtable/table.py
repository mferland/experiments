#!/usr/bin/python

def split_list(alist, chunks=1):
    length = len(alist)
    return [ alist[i*length // chunks: (i+1)*length // chunks] 
             for i in range(chunks) ]

def distribute(string, chunks):
    if chunks == len(string):
        # i.e.: 'abc' in 3 chunks ==> 'a', 'b', 'c'
        return list(string)

    if chunks == 1:
        # i.e.: 'abc' in 1 chunk ==> 'abc'
        return [string]
        
    if (chunks < len(string)):
        # i.e.: 'abcdef' in 3 chunks ==> 'ab', 'cd', 'ef'
        return sorted(split_list(string, chunks))
    else:
        # i.e.: 'abc' in 4 chunks ==> 'a', 'a', 'b', 'c',
        return sorted(list((string * (chunks // len(string) + 1))[:chunks]))

def recurse(pool, first_index, count, alist):
    if count == 1:
        return
    
    start_point = distribute(pool, count)

    i = first_index
    for s in start_point:
        alist[i].append(s)
        i+=1

    for s in sorted(set(start_point)):
        recurse(pool, start_point.index(s), start_point.count(s), alist)

def generate_table(threads, pwlen, pool):
    if threads > len(pool) ** pwlen or (threads > 1 and len(pool) == 1):
        raise AttributeError('More threads than available passwords')
    table=[]
    start_point = distribute(pool, threads)
    for s in sorted(set(start_point)):
        count = start_point.count(s)
        if count == 1:
            row = [s] + [pool] * (pwlen - 1)
            table.append(row)
        else:
            rows = [[] for x in xrange(count)]
            recurse(pool, 0, count, rows)
            for r in rows:
                r = [s] + r
                remaining = pwlen - len(r)
                if remaining > 0:
                    r = r + [pool] * remaining
                table.append(r)
    return table

def find_nearest_match(table, firstpw):
    nearest = []
    for row in table:
        tmp = ''
        for i, e in enumerate(row):
            if firstpw[i] in e:
                tmp += firstpw[i]
            elif abs(ord(firstpw[i]) - ord(e[0])) > abs(ord(firstpw[i]) - ord(e[-1])):
                tmp += e[-1]
            else:
                tmp += e[0]
        nearest.append(tmp)
    print nearest

def print_table(table):
    for row in table:
        print row

# generate_table(3, 5, 'abcdefghij')
# happy path
# print_table(generate_table(1, 1, 'a'))
# print '===='
# print_table(generate_table(1, 2, 'a'))
# print '===='
# print_table(generate_table(1, 2, 'abc'))
# print '===='
print_table(generate_table(2, 2, 'abc'))
print '===='
print_table(generate_table(2, 3, 'abc'))
print '===='
print_table(generate_table(16, 5, 'abcde'))
find_nearest_match(generate_table(16, 5, 'abcde'), 'aabbc')
# print '===='
# print_table(generate_table(8, 2, 'abc'))
# print '===='
# print_table(generate_table(9, 2, 'abc'))
# print '===='
# print_table(generate_table(9, 3, 'abc'))
# print '===='
# print_table(generate_table(9, 5, 'ab'))
# print '===='
# print_table(generate_table(12, 5, 'ab'))
