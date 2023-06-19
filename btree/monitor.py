import psutil
import time
import argparse
import matplotlib.pyplot as plt


# Parse command-line arguments
parser = argparse.ArgumentParser()
parser.add_argument('-p', '--pid', type=int, required=True, help='Process ID to monitor')
parser.add_argument('-d', '--duration', type=int, default=60, help='Duration to monitor for (in seconds)')
parser.add_argument('-f', '--file', type=str, default='workload sample', help='workload filename')
args = parser.parse_args()

def log_memory_usage(pid, duration, logfile):
    process = psutil.Process(pid)
    
    start_time = time.time()
    timestamps = []
    mem_usages = []

    with open(logfile, 'w') as f:
        while True:
            curr_time = time.time()
            mem_info = process.memory_info()
            log_message = f'Memory usage: {mem_info.rss}\n'
            f.write(log_message)
            print(log_message, end='')  # print to stdout as well
        
            timestamps.append(curr_time - start_time)
            mem_usages.append(mem_info.rss)

            if time.time() - start_time > duration:
                break
            time.sleep(1)
        # Plotting
        plt.plot(timestamps, mem_usages)
        plt.xlabel('Time (seconds)')
        plt.ylabel('Memory Usage (bytes)')
        plt.title(f'Memory usage of process {pid} over time')
        plt.savefig('../log/memory_usage_plot_packed.png')

if __name__ == '__main__':
    log_memory_usage(args.pid, args.duration, args.logfile)
