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


def getTime(output: str):
    time = re.findall('[0-9]+\.[0-9]+', output)
    if time:
        return float(time[0])

# Sample images can be downloaded here: https://www.dwsamplefiles.com/download-bmp-sample-files/

#TODO: Automatic downloading of images if not present

#TODO: Compare versions (plot csv files against each other)

#TODO: Turn this into a CLI

if __name__ == "__main__":
    os.chdir("../build")
    test_files = glob.glob("../performance/sample_images/*.bmp")
    times = []
    pixelNum = []
    if (len(sys.argv) != 2):
        sys.exit("Wrong number of args expected python performance_test.py [version]")

    version = sys.argv[1]
    for test_file in tqdm(test_files):
        process = subprocess.run(["./program", "-V", version, "-B1000", "-o", f"../performance/output_images/{test_file.split('/')[-1][:-4]}_out.bmp", test_file], stdout=subprocess.PIPE)
        output = process.stdout.decode('utf-8')
        print(output)
        times.append(getTime(output))
        pixelNum.append(get_pixel_count(test_file))

    df = pd.DataFrame({'fileName': test_files, 'time': times, 'pixelNum': pixelNum, 'version': version})
    df = df.sort_values(by=['pixelNum'])
    print(df)
    df.to_csv(f"../performance/results/testresult_preformance_{time.strftime('%Y%m%d_%H%M%S')}.csv", index=False)
    res = df.plot(x='pixelNum', y='time', style='.-').get_figure()
    res.savefig(f"../performance/graphs/testresult_version_{version}_preformance_graph_{time.strftime('%Y%m%d_%H%M%S')}.png")
    plt.show()
