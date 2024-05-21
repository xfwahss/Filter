import numpy as np
import random
from scipy.interpolate import interp1d
import os
import subprocess

random.seed(5000)
def random_generate_curve(fixed_points=50, points=3000, ymin=20, ymax=100):
    x = list(range(1, points+1))
    fixed_y = np.random.uniform(ymin, ymax, fixed_points)
    fixed_x = random.sample(x, fixed_points - 2)
    fixed_x.extend([1, points])
    f = interp1d(fixed_x, fixed_y, kind='cubic')
    y = f(x)
    return fixed_x, fixed_y, x, y

def add_noise(data, mean, std):
    nums = len(data)
    noise = np.random.normal(mean, std, nums)
    return data + noise

def sample(x, series, frequency=30):
    nums = int(len(series) / frequency)
    left = len(series) - frequency * nums
    x_sampled, series_sampled = [], []
    for i in range(nums):
        index = random.randint(0, frequency - 1)
        x_sampled.append(x[index + frequency * i])
        series_sampled.append(series[index + frequency * i])
    left_index = random.randint(0, left - 1)
    x_sampled.append(x[left_index + frequency * nums])
    series_sampled.append(series[left_index + frequency * nums])
    return np.array(x_sampled), np.array(series_sampled)

def cd_file_dir(file):
    file_dir = os.path.dirname(os.path.abspath(file))
    os.chdir(file_dir)
    print("Entering the dir:" + os.getcwd())

def compile(toggle=True):
    if(toggle):
        os.chdir("../../build")
        print("Entering the dir:" + os.getcwd())
        subprocess.run(["cmake", ".."])
        subprocess.run("make")
        os.chdir("../test/scripts")
        print("Entering the dir:" + os.getcwd())
    else:
        print("Compile skipped!")

if __name__ == "__main__":
    from matplotlib import pyplot as plt
    _, _, x, y = random_generate_curve(10, 500, 10, 100)
    plt.plot(x, y)
    x_sampled, y_sampled = sample(x, y)
    plt.plot(x_sampled, y_sampled, 'ro')
    plt.show()
