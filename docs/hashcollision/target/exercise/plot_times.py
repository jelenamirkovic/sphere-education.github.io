# author: N. H. Weideman
import argparse
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import json

def main():
    parser = argparse.ArgumentParser("Plot the times stored in a file.")
    parser.add_argument('--malicious', help='The files containing the times to store malicious strings, in json format')
    parser.add_argument('--random', help='The files containing the times to store random strings, in json format')
    parser.add_argument('--file-name', required=True, help='The name to give to the graph file')
    args = parser.parse_args()
    malicious_times_file = args.malicious
    random_times_file = args.random
    file_name = args.file_name
    with open(malicious_times_file, 'r') as fd:
            times = json.load(fd)
            axes = plt.gca()
            xs = []
            ys = []
            for i, size, time in times:
                xs.append(size)
                ys.append(time)
            axes.plot(xs, ys, label='Malicious')
    with open(random_times_file, 'r') as fd:
            times = json.load(fd)
            axes = plt.gca()
            xs = []
            ys = []
            for i, size, time in times:
                xs.append(size)
                ys.append(time)
            axes.plot(xs, ys, label='Random')
    plt.xlabel("Input Size (bytes)")
    plt.ylabel("Execution Time (s)")

    plt.legend()
    plt.savefig(file_name)

if __name__ == "__main__":
    main()
