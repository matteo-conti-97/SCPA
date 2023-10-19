import numpy as np
import time

if __name__ == "__main__":
    a = np.arange(10000000)
    b = np.arange(10000000)
    # Start the timer
    start_time = time.perf_counter()
    c = a+b
    end_time = (time.perf_counter() - start_time)*10e3
    print(f"Execution time for vector sum: {end_time:.2f} ms")
    
    n=100
    m=100
    values = np.arange(n * m)
    v=np.arange(100)
    # Reshape the 1D array into a matrix with n rows and m columns
    mat = values.reshape(n, m)
    start_time = time.perf_counter()
    result_dot = np.dot(mat, v)
    end_time = (time.perf_counter() - start_time)*10e3
    print(f"Execution time for matrix vector mul: {end_time:.2f} ms")
    
        
