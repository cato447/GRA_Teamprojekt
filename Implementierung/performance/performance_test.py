import cv2 as cv
import glob
import subprocess
import sys
import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import time
from tqdm import tqdm


def get_pixel_count(path: str):
    image = cv.imread(path)
    height, width, _ = image.shape
    return height * width


def getTimes(output: str):
    time = re.findall('[0-9]+\.[0-9]+', output)
    if time:
        exec_time = float(time[0])
        avg_exec_time = float(time[1])
        io_time = float(time[2])
        return (exec_time, avg_exec_time, io_time)

# Sample images can be downloaded here: https://www.dwsamplefiles.com/download-bmp-sample-files/

#TODO: Automatic downloading of images if not present

#TODO: Compare versions (plot csv files against each other)

#TODO: Turn this into a CLI

if __name__ == "__main__":
    os.chdir("../build")
    test_files = glob.glob("../performance/sample_images/*.bmp")
    execTimes = []
    avg_exec_times = []
    ioTimes = []
    pixelNum = []
    if (len(sys.argv) != 2):
        sys.exit("Wrong number of args expected python performance_test.py [version]")

    version = sys.argv[1]
    for test_file in test_files:
        print(f"Running program for input {test_file}")
        pixel_count = get_pixel_count(test_file)
        process = subprocess.run(["./program", "-V", version, f"-B{(1/pixel_count)*600000000}", "-o", "output.bmp", test_file], stdout=subprocess.PIPE)
        output = process.stdout.decode('utf-8')
        execTime, avg_exec_time, io_time = getTimes(output)
        avg_exec_times.append(avg_exec_time)
        execTimes.append(execTime)
        ioTimes.append(io_time)
        pixelNum.append(pixel_count)

    df = pd.DataFrame({'fileName': test_files, 'execTime': execTimes, 'avg_exec_time': avg_exec_times, 'ioTime': ioTimes, 'pixelNum': pixelNum, 'version': version})
    df = df.sort_values(by=['pixelNum'])
    print(df)
    df.to_csv(f"../performance/results/testresult_preformance_{time.strftime('%Y%m%d_%H%M%S')}.csv", index=False)
    res = df.plot(x='pixelNum', y=['avg_exec_time', 'ioTime'], style='.-').get_figure()
    res.savefig(f"../performance/graphs/testresult_version_{version}_preformance_graph_{time.strftime('%Y%m%d_%H%M%S')}.png")
    plt.show()
