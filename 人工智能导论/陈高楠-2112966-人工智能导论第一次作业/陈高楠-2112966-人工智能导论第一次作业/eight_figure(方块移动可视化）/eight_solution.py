import copy
import time
import itertools


# 生成 0 到 8 的所有排列


class Chessboard:
    def __init__(self, value, parent=0, depth=0, hscore=0, edge=3):
        self.value = value
        self.result = 1
        self.parent = parent
        self.depth = depth
        self.hscore = hscore
        self.fscore = depth + hscore
        self.edge = 3


class Option:
    def __init__(self, cb, edge):
        self.value = copy.deepcopy(cb.value)
        for i in range(edge):
            for j in range(edge):
                if self.value[i][j] == 0:
                    self.row, self.col = i, j
                    break

        self.direction = {'left': [self.col == 0, 0, -1], 'right': [self.col == edge - 1, 0, 1],
                          'up': [self.row == 0, -1, 0], 'down': [self.row == edge - 1, 1, 0]}

    def move(self, d):
        op = self.direction[d]
        if op[0]:
            return 0
        else:
            tem = self.value[self.row + op[1]][self.col + op[2]]
            self.value[self.row + op[1]][self.col + op[2]] = self.value[self.row][self.col]
            self.value[self.row][self.col] = tem
            return self.value


class Solution:
    def __init__(self, cb, edge):
        self.open = []  # open表
        self.open_a = []
        self.close_a = []
        self.open.append(cb)
        self.open_a.append(cb)
        self.close = []  # close表
        self.start = cb
        self.edge = edge
        self.target = [[edge * i + j + 1 for j in range(edge)] for i in range(edge)]
        self.target[-1][-1] = 0
        # self.target = [[1, 2, 3],  # 最后的结果
        #                [4, 5, 6],
        #                [7, 8, 0]]
        self.target_dict = {edge * i + j + 1: [i, j] for i in range(edge) for j in range(edge)}
        self.target_dict[0] = self.target_dict.pop(edge ** 2)  # Adjusting the key for 0

    def arrived(self, cb):  # 判断有没有到过
        if cb.value in self.close:
            return True
        else:
            return False

    def isDes(self, cb):
        if cb.value == self.target:
            return True
        else:
            return False

    def extend(self, cb, type=0):  # 拓展节点
        ex_direct = ['up', 'down', 'left', 'right']
        for d in ex_direct:
            temp = Option(cb, self.edge).move(d)
            if cb.parent == 0:
                tem = 0
            else:
                tem = cb.parent.value
            # print("它爹"+str(tem))
            if tem == temp:
                continue
            if temp:
                cb_son = Chessboard(temp, cb, cb.depth + 1, 0, self.edge)
                self.open.append(cb_son)

    def extend_a(self, cb, type=0):  # 拓展节点
        ex_direct = ['up', 'down', 'left', 'right']
        for d in ex_direct:
            temp = Option(cb, self.edge).move(d)
            if cb.parent == 0:
                tem = 0
            else:
                tem = cb.parent.value
            # print("它爹"+str(tem))
            insert = 0
            yes = 0
            if tem == temp:
                continue
            if temp:
                for i in self.open_a:
                    # 如果在open里面
                    if i.value == temp:
                        # 比较在open里面值的大小
                        yes = 1  # 说明在open和close里面
                        if type == 1:
                            # 好的就换掉
                            if self.h1(temp) + cb.depth + 1 < i.fscore:
                                self.open_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h1(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 5:
                            if self.h1(temp) < i.hscore:
                                self.open_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h1(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 2:
                            if self.h2(temp) + cb.depth + 1 < i.fscore:
                                self.open_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h2(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 3:
                            if self.h3(temp) + cb.depth + 1 < i.fscore:
                                self.open_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h3(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 4:
                            if self.h4(temp) + cb.depth + 1 < i.fscore:
                                self.open_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h4(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 6:
                            if cb.depth + 1 < i.depth:
                                self.open_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h4(temp), self.edge)
                                self.open_a.append(cb_son)

                for i in self.close_a:
                    if i.value == temp:
                        yes = 1
                        if type == 1:
                            if self.h1(temp) + cb.depth + 1 < i.fscore:
                                self.close_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h1(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 5:
                            if self.h1(temp) < i.hscore:
                                self.close_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h1(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 2:
                            if self.h2(temp) + cb.depth + 1 < i.fscore:
                                self.close_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h2(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 3:
                            if self.h3(temp) + cb.depth + 1 < i.fscore:
                                self.close_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h3(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 4:
                            if self.h4(temp) + cb.depth + 1 < i.fscore:
                                self.close_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h4(temp), self.edge)
                                self.open_a.append(cb_son)
                        elif type == 6:
                            if cb.depth + 1 < i.depth:
                                self.close_a.remove(i)
                                cb_son = Chessboard(temp, cb, cb.depth + 1, self.h4(temp), self.edge)
                                self.open_a.append(cb_son)

                if yes == 0:
                    if type == 0:
                        cb_son = Chessboard(temp, cb, cb.depth + 1, 0, self.edge)
                        # print("拓展的节点：",end="")# 定义一个新对象 值为返回的上行结果 父节点为ef
                        # print(cb_son.value)
                    elif type == 1 or type == 5:
                        cb_son = Chessboard(temp, cb, cb.depth + 1, self.h1(temp), self.edge)
                    elif type == 2:
                        cb_son = Chessboard(temp, cb, cb.depth + 1, self.h2(temp), self.edge)
                    elif type == 3:
                        cb_son = Chessboard(temp, cb, cb.depth + 1, self.h3(temp), self.edge)
                    elif type == 4:
                        cb_son = Chessboard(temp, cb, cb.depth + 1, self.h4(temp), self.edge)
                    elif type == 6:
                        cb_son = Chessboard(temp, cb, cb.depth + 1, self.h4(temp), self.edge)
                    self.open_a.append(cb_son)

    def h1(self, cb):
        # 用曼哈顿距离
        distance = 0

        for i in range(self.edge):
            for (j) in range(self.edge):
                d = self.target_dict[cb[i][j]]
                distance += abs(i - d[0]) + abs(j - d[1])

        return distance

    def h2(self, cb):
        distance = 0
        for i in range(self.edge):
            for (j) in range(self.edge):
                d = self.target_dict[cb[i][j]]
                distance += ((i - d[0]) ** 2 + (j - d[1]) ** 2) ** 0.5
        return distance

    def h3(self, cb):
        distance = 0
        for i in range(self.edge):
            for (j) in range(self.edge):
                d = self.target_dict[cb[i][j]]
                distance += max(abs(i - d[0]), abs(j - d[1]))

        return distance

    def h4(self, cb):
        score = 0
        for i in range(self.edge):
            for j in range(self.edge):
                score += 1 if cb[i][j] != self.target[i][j] else 0

        return score

    def exist(self):
        puzzle_1d = [number for row in self.start.value for number in row]
        x = 0
        for i in range(1, self.edge ** 2):
            if puzzle_1d[i] != 0:
                for j in range(i):
                    x += 1 if puzzle_1d[j] > puzzle_1d[i] else 0

        if x % 2 == 0:
            return 1
        else:
            return 0

    def BFS(self):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        start_time = time.perf_counter()
        all = 0
        while self.open:  # 判断openT是否空表
            all += 1
            if self.isDes(self.open[0]):
                result = {"cb": [], "depth": self.open[0].depth, "time": (time.perf_counter() - start_time),
                          "code": "Success", "search_time": all}
                pointer = self.open[0]
                result["cb"].append(pointer.value)
                while pointer.parent:
                    pointer = pointer.parent
                    result["cb"].append(pointer.value)
                result["cb"].reverse()
                return result
            else:
                # self.close.append(self.open[0].value)  # 给closeT添加值
                self.extend(self.open[0])  # 若不是结果的话则拓展ef
                self.open.pop(0)
                # 出队
            if time.perf_counter() - start_time > 6000:
                return {"cb": self.start.value, "code": "Failure", "search_time": all,
                        "time": time.perf_counter() - start_time}
        else:
            return {"cb": self.start.value, "code": "Failure", "search_time": all,
                    "time": time.perf_counter() - start_time}

    def BFS_longest(self):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        start_time = time.time()
        all = 0
        result = None
        cb = []
        depth = 0
        while self.open:  # 判断openT是否空表
            all += 1
            if self.isDes(self.open[0]):
                cb.clear()
                pointer = self.open[0]
                cb.append(pointer.value)
                depth = self.open[0].depth
                while pointer.parent:
                    pointer = pointer.parent
                    cb.append(pointer.value)
                cb.reverse()
                self.open.pop(0)


            else:
                # self.close.append(self.open[0].value)  # 给closeT添加值
                self.extend(self.open[0])  # 若不是结果的话则拓展ef
                self.open.pop(0)  # 出队
        else:
            result2 = {"cb": result["cb"], "time": time.time() - start_time,
                       "code": "Success", "search_time": all}
            return result2

    def DFS(self, depth=100):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        start_time = time.perf_counter()
        all = 0
        while self.open:  # 判断openT是否空表
            # print("拓展节点：",end="")
            # print(self.open[-1].value)
            # # print(depth,end="")
            # # print(self.open[-1].value)
            all += 1
            if self.isDes(self.open[-1]):
                result = {"cb": [], "depth": self.open[-1].depth, "time": time.perf_counter() - start_time,
                          "code": "Success", "search_time": all}
                pointer = self.open[-1]
                result["cb"].append(pointer.value)
                while pointer.parent:
                    pointer = pointer.parent
                    result["cb"].append(pointer.value)
                result["cb"].reverse()
                return result
            else:

                # self.close.append(self.open[-1].value)  # 给closeT添加值
                if self.open[-1].depth < depth:
                    a = self.open.pop()
                    self.extend(a)  # 若不是结果的话则拓展ef
                    # 出队
                else:
                    self.open.pop()
            if time.perf_counter() - start_time > 300:
                return {"cb": self.start.value, "code": "Failure", "search_time": all,
                        "time": time.perf_counter() - start_time}
        else:
            return {"cb": self.start.value, "code": "Failure2", "search_time": all,
                    "time": time.perf_counter() - start_time}

    def IDDFS(self):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        depth = 0
        all = 0
        time = 0
        while (depth < 32):
            self.open.clear()
            self.close.clear()
            self.open.append(self.start)
            result = self.DFS(depth)
            # print(result)
            if result["code"] == "Success":
                all += result["search_time"]
                result["search_time"] = all
                return result
            else:
                depth += 1
                all += result["search_time"]
                time += result["time"]
                if time > 300:
                    return {"cb": self.start.value, "code": "Failure", "time": time, "search_time": all}
                continue

    def astar1(self):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        all = 0
        start_time = time.perf_counter()
        while self.open_a:  # 判断openT是否空表且是否超过深度
            all += 1
            if self.isDes(self.open_a[0]):
                result = {"cb": [], "depth": self.open_a[0].depth, "time": time.perf_counter() - start_time,
                          "code": "Success", "search_time": all}
                pointer = self.open_a[0]
                result["cb"].append(pointer.value)
                while pointer.parent:
                    pointer = pointer.parent
                    result["cb"].append(pointer.value)
                result["cb"].reverse()
                return result
            else:
                self.close_a.append(self.open_a[0])  # 给closeT添加值
                self.extend_a(self.open_a[0], type=1)  # 若不是结果的话则拓展ef
                self.open_a.pop(0)  # 出队
                self.open_a.sort(key=lambda x: x.fscore)  # 排序 根据fscore排序
            if time.perf_counter() - start_time > 300:
                return {"value": self.start.value, "code": "Failure", "time": time.perf_counter() - start_time,
                        "search_time": all}
        else:
            return {"cb": [self.start.value], "code": "No Solution!", "time": time.perf_counter() - start_time,
                    "search_time": all}

    def astar2(self):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        all = 0
        start_time = time.perf_counter()
        while self.open_a:  # 判断openT是否空表且是否超过深度
            all += 1
            if self.isDes(self.open_a[0]):
                result = {"cb": [], "depth": self.open_a[0].depth, "time": time.perf_counter() - start_time,
                          "code": "Success", "search_time": all}
                pointer = self.open_a[0]
                result["cb"].append(pointer.value)
                while pointer.parent:
                    pointer = pointer.parent
                    result["cb"].append(pointer.value)
                result["cb"].reverse()
                return result
            else:
                self.close_a.append(self.open_a[0])  # 给closeT添加值
                self.extend_a(self.open_a[0], type=2)  # 若不是结果的话则拓展ef
                self.open_a.pop(0)  # 出队
                self.open_a.sort(key=lambda x: x.fscore)  # 排序 根据fscore排序
            if time.perf_counter() - start_time > 300:
                return {"value": self.start.value, "code": "Failure", "time": time.perf_counter() - start_time,
                        "search_time": all}
        else:
            return {"cb": [self.start.value], "code": "No Solution!", "time": time.perf_counter() - start_time,
                    "search_time": all}

    def astar3(self):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        all = 0
        start_time = time.perf_counter()
        while self.open_a:  # 判断openT是否空表且是否超过深度
            all += 1
            if self.isDes(self.open_a[0]):
                result = {"cb": [], "depth": self.open_a[0].depth, "time": time.perf_counter() - start_time,
                          "code": "Success", "search_time": all}
                pointer = self.open_a[0]
                result["cb"].append(pointer.value)
                while pointer.parent:
                    pointer = pointer.parent
                    result["cb"].append(pointer.value)
                result["cb"].reverse()
                return result
            else:
                self.close_a.append(self.open_a[0])  # 给closeT添加值
                self.extend_a(self.open_a[0], type=3)  # 若不是结果的话则拓展ef
                self.open_a.pop(0)  # 出队
                self.open_a.sort(key=lambda x: x.fscore)  # 排序 根据fscore排序
            if time.perf_counter() - start_time > 300:
                return {"value": self.start.value, "code": "Failure", "time": time.perf_counter() - start_time,
                        "search_time": all}
        else:
            return {"cb": [self.start.value], "code": "No Solution!", "time": time.perf_counter() - start_time,
                    "search_time": all}

    def astar4(self):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        all = 0
        start_time = time.perf_counter()
        while self.open_a:  # 判断openT是否空表且是否超过深度
            all += 1
            if self.isDes(self.open_a[0]):
                result = {"cb": [], "depth": self.open_a[0].depth, "time": time.perf_counter() - start_time,
                          "code": "Success", "search_time": all}
                pointer = self.open_a[0]
                result["cb"].append(pointer.value)
                while pointer.parent:
                    pointer = pointer.parent
                    result["cb"].append(pointer.value)
                result["cb"].reverse()
                return result
            else:
                self.close_a.append(self.open_a[0])  # 给closeT添加值
                self.extend_a(self.open_a[0], type=4)  # 若不是结果的话则拓展ef
                self.open_a.pop(0)  # 出队
                self.open_a.sort(key=lambda x: x.fscore)  # 排序 根据fscore排序
            if time.perf_counter() - start_time > 300:
                return {"value": self.start.value, "code": "Failure", "time": time.perf_counter() - start_time,
                        "search_time": all}
        else:
            return {"cb": [self.start.value], "code": "No Solution!", "time": time.perf_counter() - start_time,
                    "search_time": all}

    def greedy(self):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        all = 0
        start_time = time.perf_counter()
        while self.open_a:  # 判断openT是否空表且是否超过深度
            all += 1
            if self.isDes(self.open_a[0]):
                result = {"cb": [], "depth": self.open_a[0].depth, "time": time.perf_counter() - start_time,
                          "code": "Success", "search_time": all}
                pointer = self.open_a[0]
                result["cb"].append(pointer.value)
                while pointer.parent:
                    pointer = pointer.parent
                    result["cb"].append(pointer.value)
                result["cb"].reverse()
                return result
            else:
                self.close_a.append(self.open_a[0])  # 给closeT添加值
                self.extend_a(self.open_a[0], type=5)  # 若不是结果的话则拓展ef
                self.open_a.pop(0)  # 出队
                self.open_a.sort(key=lambda x: x.hscore)  # 排序 根据fscore排序
            if time.perf_counter() - start_time > 2000:
                return {"value": self.start.value, "code": "Failure", "time": time.perf_counter() - start_time,
                        "search_time": all}
        else:
            return {"cb": [self.start.value], "code": "No Solution!", "time": time.perf_counter() - start_time,
                    "search_time": all}

    def best(self):
        if self.exist() == 0:
            return {"cb": self.start.value, "code": "Failure"}
        all = 0
        start_time = time.perf_counter()
        while self.open_a:  # 判断openT是否空表且是否超过深度
            all += 1
            if self.isDes(self.open_a[0]):
                result = {"cb": [], "depth": self.open_a[0].depth, "time": time.perf_counter() - start_time,
                          "code": "Success", "search_time": all}
                pointer = self.open_a[0]
                result["cb"].append(pointer.value)
                while pointer.parent:
                    pointer = pointer.parent
                    result["cb"].append(pointer.value)
                result["cb"].reverse()
                return result
            else:
                self.close_a.append(self.open_a[0])  # 给closeT添加值
                self.extend_a(self.open_a[0], type=6)  # 若不是结果的话则拓展ef
                self.open_a.pop(0)  # 出队
                self.open_a.sort(key=lambda x: x.depth)  # 排序 根据depth排序
            if time.perf_counter() - start_time > 10000:
                return {"value": self.start.value, "code": "Failure", "time": time.perf_counter() - start_time,
                        "search_time": all}
        else:
            return {"cb": [self.start.value], "code": "No Solution!", "time": time.perf_counter() - start_time,
                    "search_time": all}


def test_DFS(result, depth=100):
    c = Chessboard(copy.deepcopy(result), edge=3)
    s = Solution(c, edge=3)
    r = s.DFS(depth)
    # print(r)
    if r["code"] == "Success":
        print(
            "DFS深度设置：" + str(depth) + "  DFS运行时间：" + str(r["time"]) + "   搜索深度：" + str(
                r["depth"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]
    else:
        print(
            "DFS运行时间：" + str(r["time"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]


def test_BFS(result):
    c = Chessboard(copy.deepcopy(result), edge=3)
    s = Solution(c, edge=3)
    r = s.BFS()
    # print(r)
    if r["code"] == "Success":
        print(
            "BFS运行时间：" + str(r["time"]) + "   搜索深度：" + str(r["depth"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]
    else:
        print(
            "BFS运行时间：" + str(r["time"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]


def test_IDDFS(result):
    c = Chessboard(copy.deepcopy(result), edge=3)
    s = Solution(c, edge=3)
    r = s.IDDFS()
    # print(r)
    if r["code"] == "Success":
        print("IDDFS运行时间：" + str(r["time"]) + "   搜索深度：" + str(r["depth"]) + "   搜索次数：" + str(
            r["search_time"]))
        return r["time"]
    else:
        print(
            "IDDFS运行时间：" + str(r["time"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]


def test_A1(result):
    c = Chessboard(copy.deepcopy(result), edge=3)
    s = Solution(c, edge=3)
    r = s.astar1()
    # print(r)
    if r["code"] == "Success":
        print(
            "A*运行时间1：" + str(r["time"]) + "   搜索深度：" + str(r["depth"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]
    else:
        print(
            "A*运行时间1：" + str(r["time"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]


def test_A2(result):
    c = Chessboard(copy.deepcopy(result), edge=3)
    s = Solution(c, edge=3)
    r = s.astar2()
    # print(r)
    if r["code"] == "Success":
        print(
            "A*运行时间2：" + str(r["time"]) + "   搜索深度：" + str(r["depth"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]
    else:
        print(
            "A*运行时间2：" + str(r["time"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]


def test_A3(result):
    c = Chessboard(copy.deepcopy(result), edge=3)
    s = Solution(c, edge=3)
    r = s.astar3()
    # print(r)
    if r["code"] == "Success":
        print(
            "A*运行时间3：" + str(r["time"]) + "   搜索深度：" + str(r["depth"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]
    else:
        print(
            "A*运行时间3：" + str(r["time"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]


def test_A4(result):
    c = Chessboard(copy.deepcopy(result), edge=3)
    s = Solution(c, edge=3)
    r = s.astar4()
    # print(r)
    if r["code"] == "Success":
        print(
            "A*运行时间4：" + str(r["time"]) + "   搜索深度：" + str(r["depth"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]
    else:
        print(
            "A*运行时间4：" + str(r["time"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]


def test_Greedy(result):
    c = Chessboard(copy.deepcopy(result), edge=3)
    s = Solution(c, edge=3)
    r = s.greedy()
    # print(r)
    if r["code"] == "Success":
        print(
            "Greedy运行时间：" + str(r["time"]) + "   搜索深度：" + str(r["depth"]) + "   搜索次数：" + str(
                r["search_time"]))
        return r["time"]
    else:
        print(
            "Greedy运行时间：" + str(r["time"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]


def test_best(result):
    c = Chessboard(copy.deepcopy(result), edge=3)
    s = Solution(c, edge=3)
    r = s.best()
    # print(r)
    if r["code"] == "Success":
        print(
            "Best运行时间：" + str(r["time"]) + "   搜索深度：" + str(r["depth"]) + "   搜索次数：" + str(
                r["search_time"]))
        return r["time"]
    else:
        print(
            "Best运行时间：" + str(r["time"]) + "   搜索次数：" + str(r["search_time"]))
        return r["time"]



