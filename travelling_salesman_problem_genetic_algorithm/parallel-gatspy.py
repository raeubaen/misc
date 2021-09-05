##THE GREAT G.A.T.S.PY
# Genetic Algoritm for Travelling Salesman problem in PYthon

import random
import pandas as pd
import matplotlib.pyplot as plt
import PySimpleGUI as sg
import numpy as np
import argparse
import os
from multiprocessing import Pool, Queue, Manager
from psutil import cpu_count
import sys

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __sub__(self, point2):
        dx = self.x - point2.x
        dy = self.y - point2.y
        dist = np.sqrt((dx ** 2) + (dy ** 2))
        return dist

class Path(list):
    @property
    def loss(self):
        length = 0
        for i in range(len(self)):
            prev_city = self[i]
            if i != len(self) - 1:
                next_city = self[i + 1]
            else:
                next_city = self[0]
            length += prev_city - next_city
        return -1/length

    # returns child - ordered crossover;
    # gene taken from parent1 (self) is inserted within parent2 and remanining basis are shifted without changing the order
    def __and__(self, parent2):
        start_ind = int(random.random() * len(self))
        shift = int(random.random() * (len(self)-start_ind))
        temp = self[start_ind : start_ind + shift]
        child = temp + [item for item in parent2 if item not in temp]
        return self.__class__(child)

    def mutate(self, mutation_rate):
        for i in range(len(self)):
            if random.random() < mutation_rate:
                swap_with = int(random.random() * len(self))
                temp = self[swap_with]
                self[swap_with] = self[i]
                self[i] = temp
        return self


class Population(list):
    def rank_paths(self):
        return self.__class__(sorted(self, key=lambda x: x.loss))

    def select(self, elite_num): #roulette wheel selection
        elite = self[:elite_num]
        total_sum = sum([p.loss for p in self])
        selection_probs = [p.loss/total_sum for p in self]
        index_selected = np.random.choice(range(len(self)), size=(len(self) - elite_num), p=selection_probs)
        selected = [self[i] for i in index_selected]
        return self.__class__(elite + selected)

    def breed(self, elite_num):
        children = self[:elite_num]
        non_elite_num = len(self) - elite_num
        non_elite = random.sample(self[elite_num:], non_elite_num)
        for i in range(non_elite_num):
            # to be sure everyone respects monogamy and every couple has exactly 2 children
            child = non_elite[i] & non_elite[non_elite_num - i - 1]
            children.append(child)
        return self.__class__(children)

    def mutate(self, mutation_rate):
        return self.__class__([path.mutate(mutation_rate) for path in self])

    def next_generation(self, elite_num, mutation_rate):
        return self.select(elite_num).breed(elite_num).mutate(mutation_rate).rank_paths()

def to_plot(path):
    return ([p.x for p in path] + [path[0].x], [p.y for p in path] + [path[0].y])

def save_path_csv(path, file_name):
    pd.DataFrame(
        [[p.x, p.y] for p in path],
        columns=["x", "y"]
    ).to_csv(os.path.join(data_path, file_name))

def evolve(population, args, queue_list, return_queue_list, island_num):
    elite_num, generations_num, mutation_rate = args.e, args.g, args.m
    test, dist, error = args.test, args.d, args.u
    immigrant_num = int(0.01 * len(population))
    for i in range(generations_num):
        population = population.next_generation(elite_num, mutation_rate) #always ordered
        if i % 20 == 0:
            queue_list[island_num].put(population[:immigrant_num])
            immigrant = queue_list[(island_num+1)%len(queue_list)].get() #provides sort of locking
            population[:immigrant_num] = immigrant
        best_path = population[0]
        if not test:
            print(f"Island {island_num} --- Gen: {i} --- Loss: {best_path.loss}", flush=True) #slows down
        elif test and abs(best_path.loss - dist) < error:
            print("CONVERGED", flush=True)
            return
    return_queue_list[island_num].put(best_path)

def gui_choose_points(points_num):
    path = Path([])
    BOX_SIZE = 25
    DIM = 450
    layout = [
        [sg.Text(f"Choose {points_num} points", key="-OUTPUT-")],
        [sg.Graph((800, 800), (0, DIM), (DIM, 0), key="-GRAPH-", change_submits=True, drag_submits=False)],
        [sg.Button("Show"), sg.Button("Exit")]
    ]
    window = sg.Window("THE GREAT G.A.T.S.PY", layout, finalize=True)
    g, t = window["-GRAPH-"], window["-OUTPUT-"]
    g.draw_rectangle((BOX_SIZE, BOX_SIZE), (BOX_SIZE, BOX_SIZE))
    while len(path) < points_num:
        event, values = window.read()
        if event in (None, "Exit"):
            break
        mouse = values["-GRAPH-"]
        if event == "-GRAPH-":
            if mouse == (None, None):
                continue
            box_x, box_y = mouse[0], mouse[1]
            path.append(Point(1 - box_x / DIM, 1 - box_y / DIM))
            g.draw_circle((box_x, box_y), 5, fill_color="black", line_color="white")
            t.update(value=f"Choose {points_num - len(path)} points")
    window.close()
    return path

def quit(test, *args, **kwargs):
    if test:
        p.terminate()

def error_callback(exception, *args, **kwargs):
    print(exception, flush=True)

def main():
    global p, data_path
    parser = argparse.ArgumentParser(description="THE GREAT G.A.T.S.PY --- Genetic Algoritm for Travelling Salesman problem in PYthon")
    parser.add_argument("--rand", default=False, action="store_true", help="extract points randomly (default: False)")
    parser.add_argument("--file", default=None, help="takes initial path from file (.csv with cols index-x-y) (default: None)")
    parser.add_argument("--save", default=False, action="store_true", help="saves pictures and best path data (default: False)")
    parser.add_argument("-p", type=int, default=30, help="number of points when extracted randomly (default: 30)")
    parser.add_argument("-s", type=int, default=100, help="size of populations (default: 100)")
    parser.add_argument("-g", type=int, default=1000, help="number of generations (default: 700)")
    parser.add_argument("-e", type=int, default=30, help="number of elite individuals (default: 30)")
    # elite size better near 30%
    parser.add_argument("-m", type=float, default=0.005, help="mutation rate (default: 0.005)")
    # if it's 0, convergence is faster but increases probability to get stuck in local minima
    # over a certain value the algorithm could stop converging
    parser.add_argument("--test", default=False, action="store_true", help="stops the program when true distance is reached within a predefined error")
    parser.add_argument("-d", type=float, help="true distance, used if testing")
    parser.add_argument("-u", type=float, help="range of error allowed during testing")
    args = parser.parse_args()

    if args.file != None:
        df = pd.read_csv(args.file)
        df.rename(columns={ df.columns[1]: "x", df.columns[2]: "y"})
        initial_path = Path([])
        for index, row in df.iterrows():
            initial_path.append(Point(row.x, row.y))
    elif not args.rand:
        initial_path = gui_choose_points(args.p)
    else:
        rand_x, rand_y = np.random.rand(args.p,), np.random.rand(args.p,)
        initial_path = Path([Point(x=rand_x[i], y=rand_y[i]) for i in range(args.p)])

    random.shuffle(initial_path)
    if args.save:
        if not os.path.exists("data"):
            os.makedirs("data")
        data_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
        save_path_csv(initial_path, "initial_path.csv")

    #starting parallel code (lucky Carlo)
    usable_cpu = cpu_count(logical=False) -1 #for Ruben it's 1 :( POOR RUBEN !!1!
    initial_population = Population([initial_path] * int(args.s))
    manager = Manager()
    queue_list, return_queue_list = [manager.Queue()]*usable_cpu, [manager.Queue()]*usable_cpu
    evolve_args_partial_list = [initial_population, args, queue_list, return_queue_list]

    stop_callback = lambda x: quit(args.test)
    p = Pool(usable_cpu)
    for i in range(usable_cpu):
        p.apply_async(evolve, args=tuple(evolve_args_partial_list + [i]), callback=stop_callback, error_callback=error_callback)
    p.close()
    p.join()
    if not args.test:
        final_path = Population([q.get() for q in return_queue_list]).rank_paths()[0]
        print(final_path)
        data = to_plot(final_path)
        plt.plot(data[0], data[1])
        plt.show()

if __name__ == "__main__":
    main()