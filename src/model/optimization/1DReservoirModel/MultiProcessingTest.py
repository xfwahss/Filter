from multiprocessing import Process, Value, Array, Pool
import multiprocessing as mp
import time

core_nums = int(mp.cpu_count())

def f(n, a):
    n.value = 3.1415927
    for i in range(len(a)):
        a[i] = -a[i]

def task_with_index(index, arg):
    print(f"Process {index} is processing {arg}")
    v = arg + 10
    time.sleep(0.1)
    return v


if __name__ == '__main__':
    args = list(range(1000))
    start_time = time.time()
    with Pool(core_nums) as pool:
        results = [pool.apply_async(task_with_index, (i, arg)) for i, arg in enumerate(args)]
        for i, r in enumerate(results):
            args[i] = r.get()
    end_time = time.time()
    v1_time = end_time - start_time

    start_time = time.time()
    for i, v in enumerate(args):
        args[i] = v + 1
        time.sleep(0.1)
    end_time = time.time()
    v2_time = end_time - start_time
    print(v1_time, v2_time)
