import psutil
import time
import argparse
import subprocess
import matplotlib.pyplot as plt


def log_memory_usage(program, duration, logfile, pltfile):
    timestamps = []
    mem_usages = []
    with open(logfile, 'w+') as f:
        # Start the program as a child process
        process = subprocess.Popen(program)
        pid = process.pid

        # Monitor the child process
        proc = psutil.Process(pid)
        start_time = time.time()
        while True:
            if proc.is_running():
                curr_time = time.time()
                try:
                    mem_info = proc.memory_info()
                    log_message = f'Time: {curr_time-start_time}, Memory usage: {mem_info.rss}\n'
                    f.write(log_message)
                    print(log_message, end='')  # print to stdout as well

                    # Add current time and memory usage to lists
                    timestamps.append(curr_time - start_time)
                    mem_usages.append(mem_info.rss)
                except:
                    break

                if curr_time - start_time > duration:
                    break
                time.sleep(0.5)
            else:
                break
        plt.plot(timestamps, mem_usages)
        plt.xlabel('Time (seconds)')
        plt.ylabel('Memory Usage (bytes)')
        plt.title(f'Memory usage of process {pid} over time')
        plt.savefig(pltfile)


    # Plotting
    plt.plot(timestamps, mem_usages)
    plt.xlabel('Time (seconds)')
    plt.ylabel('Memory Usage (bytes)')
    plt.title(f'Memory usage of process {pid} over time')
    plt.savefig('memory_usage_plot.png')

    # Make sure to terminate the child process if it's still running after monitoring is done
    if process.poll() is None:
        process.terminate()

if __name__ == "__main__":
    dir_path = "log/"
    # Parse command-line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--program', type=str, required=True, help='Path to the executable to run and monitor')
    parser.add_argument('-d', '--duration', type=int, default=15, help='Duration to monitor for (in seconds)')
    # parser.add_argument('-f', '--logfile', type=str, default='../log/memory_log.txt', help='Log file to write memory usage to')
    args = parser.parse_args()

    logfile = dir_path + args.program + '_log.txt'
    imgfile = dir_path + args.program + '_plot.png'

    log_memory_usage(args.program, args.duration, logfile, imgfile)

