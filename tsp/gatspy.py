##THE GREAT G.A.T.S.PY
# Genetic Algoritm for Travelling Salesman problem in PYthon

import random
import pandas as pd
import matplotlib.pyplot as plt
import PySimpleGUI as sg
import numpy as np
import argparse
import os

if not os.path.exists("data"):
    os.makedirs("data")
data_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")

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
        ind1 = int(random.random() * len(self))
        ind2 = int(random.random() * len(self))
        start_ind, end_ind = min(ind1, ind2), max(ind1, ind2)
        temp = self[start_ind : end_ind]
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
        return self.rank_paths().select(elite_num).breed(elite_num).mutate(mutation_rate)

def to_plot(path):
    return ([p.x for p in path] + [path[0].x], [p.y for p in path] + [path[0].y])

def save_path_csv(path, file_name):
    pd.DataFrame(
        [[p.x, p.y] for p in path], 
        columns=["x", "y"]
    ).to_csv(os.path.join(data_path, file_name))

def evolve(population, generations_num=700, elite_num=30, mutation_rate=0.005, to_save=True):
    print(f"Initial distance: { - 1/population.rank_paths()[0].loss}")
    distances = []
    fig, ax = plt.subplots()
    data = to_plot(population[0])
    lines = ax.plot(data[0], data[1], marker="o")
    fig.canvas.manager.show()
    for i in range(generations_num):
        population = population.next_generation(elite_num, mutation_rate)
        best_path = population.rank_paths()[0]
        distances.append(best_path.loss)
        print(f"Gen: {i} --- Loss: {best_path.loss}")
        if i % 20 == 0:
            data = to_plot(best_path)
            lines[0].set_data(data[0], data[1])
            fig.canvas.draw()
            fig.canvas.flush_events()
    if to_save: 
        plt.savefig(os.path.join(data_path, "final_path.png"))
        save_path_csv(best_path, "final_path.csv")
    plt.close()
    plt.plot(range(len(distances)), distances)
    plt.ylabel("Distance")
    plt.xlabel("Generation")
    plt.ion()
    plt.show()
    if to_save: plt.savefig(os.path.join(data_path, "progress.png"))
    plt.pause(10)
    print(f"Final distance: {- 1 / best_path.loss}")
    return best_path

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


parser = argparse.ArgumentParser(description="THE GREAT G.A.T.S.PY --- Genetic Algoritm for Travelling Salesman problem in PYthon")
parser.add_argument("--rand", default=False, action="store_true", help="extract points randomly (default: False)")
parser.add_argument("--save", default=False, action="store_true", help="saves pictures and best path data (default: False)")
parser.add_argument("-p", type=int, default=30, help="number of points when extracted randomly (default: 30)")
parser.add_argument("-s", type=int, default=100, help="size of populations (default: 100)")
parser.add_argument("-g", type=int, default=700, help="number of generations (default: 700)")
parser.add_argument("-e", type=int, default=30, help="number of elite individuals (default: 30)")
# elite size better near 30%
parser.add_argument("-m", type=float, default=0.005, help="mutation rate (default: 0.005)")
# if it's 0, convergence is faster but increases probability to get stuck in local minima
# over a certain value the algorithm could stop converging
args = parser.parse_args()

if not args.rand: 
    initial_path = gui_choose_points(args.p)
else:
    rand_x, rand_y = np.random.rand(args.p,), np.random.rand(args.p,)
    initial_path = Path([Point(x=rand_x[i], y=rand_y[i]) for i in range(args.p)])

if args.save: save_path_csv(initial_path, "initial_path.csv")
initial_population = Population([initial_path] * args.s)
evolve(initial_population, generations_num=args.g, elite_num=args.e, mutation_rate=args.m, to_save=args.save)
