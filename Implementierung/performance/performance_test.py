import cv2 as cv
import glob
import subprocess
import sys
import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import time


def get_pixel_count(path: str):
    image = cv.imread(path)
    height, width, _ = image.shape
    return height * width


def getTimes(output: str):
    time = re.findall('[0-9]+\.[0-9]+', output)
    if time:
        io_read_time = float(time[0])
        exec_time = float(time[1])
        io_write_time = float(time[2])
        avg_exec_time = float(time[3])
        avg_io_read_time = float(time[4])
        avg_io_write_time = float(time[5])
        return (io_read_time, io_write_time,exec_time, avg_exec_time, avg_io_read_time, avg_io_write_time)

# Sample images can be downloaded here: https://www.dwsamplefiles.com/download-bmp-sample-files/

#TODO: Automatic downloading of images if not present

#TODO: Compare versions (plot csv files against each other)

#TODO: Turn this into a CLI

if __name__ == "__main__":
    os.chdir("../build")
    test_files = glob.glob("../performance/sample_images/*.bmp")
    execTimes = []
    avg_exec_times = []
    ioWriteTimes = []
    avg_io_write_times = []
    avg_io_read_times = []
    ioReadTimes = []
    pixelNum = []
    if (len(sys.argv) != 2):
        sys.exit("Wrong number of args expected python performance_test.py [version]")

    version = sys.argv[1]
    for test_file in test_files:
        print(f"Running program for input {test_file}")
        pixel_count = get_pixel_count(test_file)
        cycles = int((1/pixel_count)*10000000000)
        if pixel_count < 5000:
             cycles = 50000
        process = subprocess.run(["./program", "-V", version, f"-B{cycles}", "-o", f"../performance/output_images/{test_file.split('/')[-1]}", test_file], stdout=subprocess.PIPE)
        output = process.stdout.decode('utf-8')
        if process.returncode != 0:
            print(output)
            exit(1)
        if "FAIL testSimilarity" in output:
            print(f"Error on file {test_file}")
            print("================== Output ==================")
            print(output)
            exit(1)
        ioReadTime, ioWriteTime, execTime, avg_exec_time, avg_io_read_time, avg_io_write_time = getTimes(output)
        avg_exec_times.append(avg_exec_time)
        execTimes.append(execTime)
        avg_io_write_times.append(avg_io_write_time)
        ioWriteTimes.append(ioWriteTime)
        avg_io_read_times.append(avg_io_read_time)
        ioReadTimes.append(ioReadTime)
        pixelNum.append(pixel_count)
        print(f"ioReadTime: {ioReadTime}, ioWriteTime: {ioWriteTime}, execTime: {execTime}")

    df = pd.DataFrame({'fileName': test_files, 'execTime': execTimes, 'avg_exec_time': avg_exec_times,
                       'ioReadTime': ioReadTimes, 'avg_io_read_time': avg_io_read_times,
                       'ioWriteTime': ioWriteTimes, 'avg_io_write_time': avg_io_write_times,
                       'pixelNum': pixelNum, 'version': version})
    df = df.sort_values(by=['pixelNum'])
    print(df)
    df.to_csv(f"../performance/results/testresult_preformance_{time.strftime('%Y%m%d_%H%M%S')}.csv", index=False)
    res = df.plot(x='pixelNum', y=['avg_exec_time', 'avg_io_write_time', 'avg_io_read_time'], style='.-', title=f"Performance Version {version}").get_figure()
    res.savefig(f"../performance/graphs/testresult_version_{version}_preformance_graph_{time.strftime('%Y%m%d_%H%M%S')}.png")
    plt.show()
