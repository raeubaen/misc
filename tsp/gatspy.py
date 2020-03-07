import random
import pandas as pd
import matplotlib.pyplot as plt
from multiprocessing import Process
import PySimpleGUI as sg
import numpy as np
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __sub__(self, city):
        dx = self.x - city.x
        dy = self.y - city.y
        dist = np.sqrt((dx ** 2) + (dy ** 2))
        return dist

    def __repr__(self):
        return f"({self.x}, {self.y})"


class Path(list):
    @property
    def length(self):
        length = 0
        cities_num = len(self)
        for i in range(cities_num):
            prev_city = self[i]
            if i != cities_num - 1:
                next_city = self[i + 1]
            else:
                next_city = self[0]
            length += prev_city - next_city
        return length

    @property
    def loss(self):
        return -1 / self.length


    #returns child - ordered crossover;
    #gene taken from parent1 is inserted within parent2 and remanining basis are shifted without changing the order
    def __and__(self, parent2): 

        child, temp= [], []
        
        ind1 = int(random.random() * len(self))
        ind2 = int(random.random() * len(self))

        start_ind = min(ind1, ind2)
        end_ind = max(ind1, ind2)

        for i in range(start_ind, end_ind):
            temp.append(self[i])
        
        child = temp + [item for item in parent2 if item not in temp]
        
        return self.__class__(child)
    #----------------------------------------------

    def mutate(self, mutation_rate):
        for swapped in range(len(self)):
            if random.random() < mutation_rate:
                swap_with = int(random.random() * len(self))
                temp = self[swap_with]
                self[swap_with] = self[swapped]
                self[swapped] = temp
        return self


class Population(list):
    def reconvert(self, list_object):
        new_instance = self.__class__(list_object)
        new_instance.elite_num = self.elite_num
        new_instance.mutation_rate = self.mutation_rate
        return new_instance

    def set_params(self, elite_num=None, mutation_rate=None):
        self.elite_num, self.mutation_rate = elite_num, mutation_rate
        if elite_num is None:
            self.elite_num = len(self) // 5
        if mutation_rate is None:
            self.mutation_rate = 0.01

    @property
    def total_loss(self):
        total_loss = 0
        for path in self:
            total_loss += path.loss
        return - total_loss

    def rank_paths(self):
        sorted_population = self.__class__(sorted(self, key=lambda x: x.loss))
        return self.reconvert(sorted_population)

    def select(self): 
        elite_num = self.elite_num
        selected = self[:elite_num]
        s = {i: self[i].loss for i in range(len(self))}
        df = pd.DataFrame(s.items(), columns=["", "loss"])
        df['cumulative_sum'] = df.loss.cumsum()
        df['freq'] = df.cumulative_sum/df.loss.sum() #weights individuals loss over the whole population
        #higher loss - more copies
        for i in range(len(self) - elite_num): 
            rand = random.random()
            for i in range(len(self)):
                if df.iat[i, 3] > rand: #iac is 10x faster then iloc[i]['freq']
                    selected.append(self[i])
                    break
        return self.reconvert(selected)

    def breed(self):
        elite_num = self.elite_num
        children = self[:elite_num]
        non_elite = random.sample(self, len(self))
        length = len(self) - elite_num
        for i in range(length):
            #to be sure everyone respects monogamy and every couple has exactly 2 children
            child = non_elite[i] & non_elite[len(self)-i-1]
            children.append(child)
        return self.reconvert(children)

    def mutate(self):
        mutation_rate = self.mutation_rate
        mutated_population = [path.mutate(mutation_rate) for path in self]
        return self.reconvert(mutated_population)

    def next_generation(self):
        return self.rank_paths().select().breed().mutate()

def plot(x, y):
    plt.plot(x, y)
    plt.show()

def evolve(population, generations_num):
    print(f"Initial distance: { - 1/population.rank_paths()[0].loss}")
    distances = []
    best_path = population.rank_paths()[0]
    for i in range(generations_num):
        population = population.next_generation()
        best_path = population.rank_paths()[0]
        distances.append(best_path.loss)
        print(f"Gen: {i} --- Loss: {best_path.loss}")
        if i%50 == 49:
            if i > 50:
                p.terminate()
            p = Process(target=plot, args=([p.x for p in best_path], [p.y for p in best_path]))
            p.start()
    plt.plot(distances)
    plt.ylabel("Distance")
    plt.xlabel("Generation")
    plt.show()
    print(f"Final distance: {- 1 / best_path.loss}")
    return best_path


points_num = 50
population_size = 100
generations_num = 700
elite_num = 50 #better near 30%
mutation_rate = 0.00 #if it's 0, convergence is faster but progress curve is a broken line

BOX_SIZE = 25

layout = [
    [sg.Text(f'Select {points_num} points'), sg.Text('', key='-OUTPUT-')],
    [sg.Graph((800, 800), (0, 450), (450, 0), key='-GRAPH-',
              change_submits=True, drag_submits=False)],
    [sg.Button('Show'), sg.Button('Exit')]
]

window = sg.Window('Genetic Algorithm for TSP', layout, finalize=True)

g = window['-GRAPH-']

g.draw_rectangle((BOX_SIZE, BOX_SIZE), (BOX_SIZE, BOX_SIZE))

while True:             # Event Loop
    event, values = window.read()
    print(event, values)
    if event in (None, 'Exit'):
        break
    mouse = values['-GRAPH-']

    if event == '-GRAPH-':
        if mouse == (None, None):
            continue
        box_x = mouse[0]/BOX_SIZE
        box_y = mouse[1]/BOX_SIZE
        print(box_x, box_y)

window.close()

initial_path = Path( 
    [
        Point(
            x=int(random.random() * 200), 
            y=int(random.random() * 200)
        ) 
        for i in range(points_num)
    ]
)

initial_population = Population([initial_path]*population_size)
initial_population.set_params(elite_num=elite_num, mutation_rate=mutation_rate)
evolve(initial_population, generations_num)
