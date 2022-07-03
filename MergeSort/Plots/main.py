
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('seaborn-whitegrid')

points = {
    '4_threads': {
        'x': [],
        'y': []
    },

    '2_threads': {
        'x': [],
        'y': []
    },

    'sequential': {
        'x': [],
        'y': []
    },

    'acceleration_2_threads': {
        'x': [],
        'y': []
    },

    'acceleration_4_threads': {
        'x': [],
        'y': []
    }
}

with open('time_concurrent_4_threads.txt', 'r') as file:
    lines = file.readlines()
    for line in lines:
        data = line.split(',')
        data[0] = data[0].replace('(', '')
        data[1] = data[1].replace(')', '').replace(' ', '').replace('\n', '')

        points['4_threads']['x'].append(float(data[0]))
        points['4_threads']['y'].append(float(data[1]))

with open('time_concurrent_2_threads.txt', 'r') as file:
    lines = file.readlines()
    for line in lines:
        data = line.split(',')
        data[0] = data[0].replace('(', '')
        data[1] = data[1].replace(')', '').replace(' ', '').replace('\n', '')

        points['2_threads']['x'].append(float(data[0]))
        points['2_threads']['y'].append(float(data[1]))

with open('time_sequential.txt', 'r') as file:
    lines = file.readlines()
    for line in lines:
        data = line.split(',')
        data[0] = data[0].replace('(', '')
        data[1] = data[1].replace(')', '').replace(' ', '').replace('\n', '')

        points['sequential']['x'].append(float(data[0]))
        points['sequential']['y'].append(float(data[1]))

for (x, s, p_2, p_4) in zip(points['sequential']['x'], points['sequential']['y'], points['2_threads']['y'], points['4_threads']['y']):
    points['acceleration_2_threads']['x'].append(x)
    points['acceleration_2_threads']['y'].append(s/p_2)

    points['acceleration_4_threads']['x'].append(x)
    points['acceleration_4_threads']['y'].append(s/p_4)

"""
Descomentar as duas linhas abaixo para gerar o gráfico de aceleração.
"""
# plt.plot(np.array(points['acceleration_2_threads']['x']), np.array(points['acceleration_2_threads']['y']), '-ok')
# plt.plot(np.array(points['acceleration_4_threads']['x']), np.array(points['acceleration_4_threads']['y']), '-ok', color='blue')

"""
Plotando o tempo sequencial, com 2 threads e com 4 threads"""
plt.plot(np.array(points['sequential']['x']), np.array(points['sequential']['y']), '-ok', color='red')
plt.plot(np.array(points['2_threads']['x']), np.array(points['2_threads']['y']), '-ok', color='blue')
plt.plot(np.array(points['4_threads']['x']), np.array(points['4_threads']['y']), '-ok', color='black')

# plt.legend(['Aceleração 2 threads', 'Aceleração 4 threads'])
plt.legend(['Sequencial', '2 threads', '4 threads'])
plt.show()
