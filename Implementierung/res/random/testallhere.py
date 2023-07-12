import os
import subprocess

ordner = os.getcwd()
bmps = [bmp for bmp in os.listdir(ordner) if bmp.endswith('.bmp')]

for bmp in bmps:
    subprocess.run(["../../build/program", "-V", "1", "-B10", "-o", "temp.bmp", bmp])

input()