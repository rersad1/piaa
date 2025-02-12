import matplotlib.pyplot as plt
import re

def read_data(filename):
    n_values = []
    time_values = []
    with open(filename, 'r') as file:
        for line in file:
            match = re.match(r'N = (\d+): ([\d.]+) ms', line)
            if match:
                n_values.append(int(match.group(1)))
                time_values.append(float(match.group(2)))
    return n_values, time_values

def plot_graph(n_values, time_values):
    plt.figure(figsize=(12, 6))
    plt.plot(n_values, time_values, marker='o')
    plt.xlabel('N (Размер стороны квадрата)')
    plt.ylabel('Время выполнения (мс)')
    plt.title('Зависимость времени выполнения от размера стороны квадрата')
    plt.grid(True)
    plt.xticks(n_values)
    plt.yscale('log')
    plt.savefig('execution_time_graph.png')
    plt.show()

if __name__ == "__main__":
    filename = "execution_times.txt"
    n_values, time_values = read_data(filename)
    plot_graph(n_values, time_values)