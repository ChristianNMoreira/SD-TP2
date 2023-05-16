import subprocess
import numpy as np
import matplotlib.pyplot as plt

def run_c_code(vector_size, num_threads):
    cmd = ['./a.out']
    cmd.append(str(vector_size))
    cmd.append(str(num_threads))
    result = subprocess.run(cmd, capture_output=True, text=True)
    output = result.stdout
    lines = output.split('\n')
    t_threads = float(lines[5].split(':')[1].strip())
    return t_threads

vector_sizes = [int(1e7), int(1e8), int(1e9)]  # Valores de N
num_threads_values = [ 1, 2, 4, 8, 16, 32, 64, 128, 256]  # Valores do número de threads
num_runs = 10  # Número de vezes que o código será executado para cada configuração

average_times = np.zeros((len(vector_sizes), len(num_threads_values)))

for i, vector_size in enumerate(vector_sizes):
    for j, num_threads in enumerate(num_threads_values):
        times = []
        for _ in range(num_runs):
            t_threads = run_c_code(vector_size, num_threads)
            times.append(t_threads)
        average_time = np.mean(times)
        average_times[i][j] = average_time

# Plotando o gráfico
for i, vector_size in enumerate(vector_sizes):
    plt.plot(num_threads_values, average_times[i], label=f'N = {vector_size}')

plt.xlabel('Número de Threads')
plt.ylabel('Tempo Médio de Execução (s)')
plt.title('Tempo Médio de Execução em Função do Número de Threads')
plt.legend()
plt.show()
