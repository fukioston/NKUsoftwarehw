import copy
import time
from django.http import JsonResponse
from django.shortcuts import render
import eight_solution


# Create your views here.
def solve(request):
    # result1= [[1, 2, 3],
    #           [8, 6, 4],
    #           [7, 0, 5]]
    #
    # c = eight_solution.Chessboard(copy.deepcopy(result1))
    # result=eight_solution.Solution(c).BFS()
    # print(result)
    return render(request, 'solution.html')


def get_result(request):
    print(request.GET)
    # start0 = request.GET.getlist('start[0][]')
    # start1 = request.GET.getlist('start[1][]')
    # start2 = request.GET.getlist('start[2][]')
    # start = [start0, start1, start2]
    selectedValue = request.GET.get('selectedValue')
    print(selectedValue)
    start = [list(map(int, request.GET.getlist(f'start[{i}][]'))) for i in range(3)]
    print(start)
    c = eight_solution.Chessboard(copy.deepcopy(start))
    if selectedValue == 'BFS':
        result = eight_solution.Solution(c, 3).BFS()
    elif selectedValue == 'DFS':
        result = eight_solution.Solution(c, 3).DFS()
    elif selectedValue == 'IDDFS':
        result = eight_solution.Solution(c, 3).IDDFS()
    elif selectedValue == 'A1':
        result = eight_solution.Solution(c, 3).astar1()
    elif selectedValue == 'A2':
        result = eight_solution.Solution(c, 3).astar2()
    elif selectedValue == 'A3':
        result = eight_solution.Solution(c, 3).astar3()
    elif selectedValue == 'A4':
        result = eight_solution.Solution(c, 3).astar4()
    elif selectedValue == 'Best':
        result = eight_solution.Solution(c, 3).best()
    else:
        result = eight_solution.Solution(c, 3).greedy()
    if result.get('code') == 'Failure':
        return JsonResponse(
            {'status': 'failure', 'message': '失败'})
    return JsonResponse(
        {'depth': result.get('depth'), 'search_time': result.get('search_time'), 'result': result.get('cb'),
         'time': result.get('time'), 'status': 'success', 'message': '成功'})
