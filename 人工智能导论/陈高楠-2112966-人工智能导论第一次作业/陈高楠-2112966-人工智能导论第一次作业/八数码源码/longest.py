import copy
import itertools
import eight

permutations = itertools.permutations(range(9))

matrices = [[list(p[i:i + 3]) for i in range(0, len(p), 3)] for p in permutations]
a = 0
b = []
time = 0
for i in range(100):
    b.append([])
for matrix in matrices:  
    time += 1
    if time % 1000 == 0:
        print(time)
    m = matrix
    c = eight.Chessboard(copy.deepcopy(m), edge=3)
    s = eight.Solution(c, edge=3)
    r = s.astar1()
    if r["code"] == "Success":
        if r["depth"] > a:
            a = r["depth"]
        b[int(r["depth"])].append(m)


