from multiprocessing import Process, Value, Array, Pool
import multiprocessing as mp
import time

core_nums = int(mp.cpu_count())

print(core_nums)
