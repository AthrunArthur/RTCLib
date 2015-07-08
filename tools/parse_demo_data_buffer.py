##Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.07.07

import os
import sys
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import time
from common import *


time_stamp = str(time.time())
time_stamp = time_stamp[0: time_stamp.find('.')]

def read_buffer_profile(fp):
    res = []
    for line in open(fp):
        ls = line.strip().split(' ')
        if len(ls) != 3:
            print line
            continue

        res.append((int(ls[0]), int(ls[1]), int(ls[2])))
    return res

def parse_buffer_profile(data):
    '''
    @data - [(time, event_id, event_value)]
    @time - micro seconds
    @event_id - 1:e_in, 2: e_out, 3: e_drop, 4:e_recycle
    @event_value - data id
    '''
    tres = {}
    for d in data:
        time = d[0]
        e = d[1]
        id = d[2]
        if not tres.has_key(id):
            tres[id] = [0, 0, 0, 0, 0]
        tres[id][e] = time

    res = []
    for k, v in sorted(tres.items(), key=lambda x: x[0]):
        res.append((k, v))
    return res

def brief_report(profile_data):
    print 'Total frames: ' + str(len(profile_data))

    parsed_data = [x for x in profile_data if x[1][2] != 0]
    print 'Parsed frames: ' + str(len(parsed_data))

    droped_data = [x for x in profile_data if x[1][3] != 0]
    print 'Droped frames: ' + str(len(droped_data))

def plot_arrive_interval(profile_data):

    pd2 = profile_data[1:]
    pd2.append(profile_data[0])
    ds = [x[1][1][1] - x[0][1][1] for x in zip(profile_data, pd2)]

    ds = ds[0:len(ds) - 1]
    avg_ds = 1.0 * sum(ds) / len(ds)
    print 'Average arrive interval: ' + str(avg_ds)
    xs = range(1, len(ds) + 1)
    fp = os.path.join(output_dir, "arrive_interval" + time_stamp + ".pdf")
    with PdfPages(fp) as pdf:
        plt.plot(xs, ds)
        plt.title("The interval of frames' arriving")
        plt.xlabel("frames")
        plt.ylabel("times (micro second, ms)")
        pdf.savefig()
        plt.close()


def plot_out_interval(profile_data):
    parsed_data = [x for x in profile_data if x[1][2] != 0]
    pd2 = parsed_data[1:]
    pd2.append(parsed_data[0])
    ds = [x[1][1][2] - x[0][1][2] for x in zip(parsed_data, pd2)]

    ds = ds[0:len(ds) - 1]
    avg_ds = 1.0 * sum(ds) / len(ds)
    print 'Average out interval: ' + str(avg_ds)
    xs = range(1, len(ds) + 1)
    fp = os.path.join(output_dir, "out_interval" + time_stamp + ".pdf")
    with PdfPages(fp) as pdf:
        plt.plot(xs, ds)
        plt.title("The interval of frames' getting out from the buffer")
        plt.xlabel("frames")
        plt.ylabel("times (micro second, ms)")
        pdf.savefig()
        plt.close()



def plot_queue_time(profile_data):
    parsed_data = [x for x in profile_data if x[1][2] != 0]

    ds = [x[1][2] - x[1][1] for x in parsed_data]

    avg_ds = 1.0 * sum(ds) / len(ds)
    print 'Average queue time: ' + str(avg_ds)
    xs = range(1, len(ds) + 1)
    fp = os.path.join(output_dir, "queue_time" + time_stamp + ".pdf")
    with PdfPages(fp) as pdf:
        plt.plot(xs, ds)
        plt.title("The life-time (queue_time) of each frame")
        plt.xlabel("frames")
        plt.ylabel("times (micro second, ms)")
        pdf.savefig()
        plt.close()


def plot_process_time(profile_data):
    parsed_data = [x for x in profile_data if x[1][2] != 0]

    ds = [x[1][4] - x[1][2] for x in parsed_data]

    avg_ds = 1.0 * sum(ds) / len(ds)
    print 'Average process time: ' + str(avg_ds)
    xs = range(1, len(ds) + 1)
    fp = os.path.join(output_dir, "process_time" + time_stamp + ".pdf")
    with PdfPages(fp) as pdf:
        plt.plot(xs, ds)
        plt.title("Each frame's process time")
        plt.xlabel("frames")
        plt.ylabel("times (micro second, ms)")
        pdf.savefig()
        plt.close()

def plot_drop_time(profile_data):
    droped_data = [x for x in profile_data if x[1][3] != 0]

    if len(droped_data) == 0:
        return
    ds = [x[1][3] - x[1][1] for x in droped_data]


    avg_ds = 1.0 * sum(ds) / len(ds)
    print 'Average process time: ' + str(avg_ds)
    xs = range(1, len(ds) + 1)
    fp = os.path.join(output_dir, "drop_time" + time_stamp + ".pdf")
    with PdfPages(fp) as pdf:
        plt.plot(xs, ds)
        plt.title("Each frame's drop time")
        plt.xlabel("frames")
        plt.ylabel("times (micro second, ms)")
        pdf.savefig()
        plt.close()



def main():
    for arg in sys.argv[1:]:
        print arg
        data = read_buffer_profile(arg)
        profile_data = parse_buffer_profile(data)
        brief_report(profile_data)
        plot_arrive_interval(profile_data)
        plot_out_interval(profile_data)
        plot_queue_time(profile_data)
        plot_process_time(profile_data)
        plot_drop_time(profile_data)

if __name__ == "__main__":
    main()
