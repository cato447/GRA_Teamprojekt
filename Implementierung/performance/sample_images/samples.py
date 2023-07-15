import random
import subprocess
import os
from PIL import Image

print("Dieses Skript generiert BMP-Bilder mit gängigen Größen.")

width = 5120
height = 2880

pixel = [(0, 0, 0)] * width * height

for i in range(0, width * height):
    pixel[i] = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

# generate() mit ChatGPT generiert
def generate(w, h):
    global pixel
    img = Image.new("RGB", (w, h))
    img.putdata(pixel[:w*h])
    img.save("random" + str(w) + "x" + str(h) + ".bmp")

generate(16, 16)
generate(24, 24)
generate(32, 32)
generate(48, 48)
generate(64, 64)
generate(128, 128)
generate(256, 256)
generate(320, 240)
generate(480, 320)
generate(640, 480)
generate(800, 600)
generate(1024, 768)
generate(1280, 720)
generate(1366, 768)
generate(1600, 900)
generate(1920, 1080)
generate(2048, 1152)
generate(2560, 1440)
generate(2880, 1620)
generate(3200, 1800)
generate(3840, 2160)
generate(4096, 2160)
generate(5120, 2880)
generate(640, 360)
generate(720, 480)
generate(800, 450)
generate(960, 540)
generate(1024, 576)
generate(1280, 800)
generate(1440, 900)
generate(1680, 1050)
generate(1920, 1200)
generate(2560, 1600)
generate(2880, 1800)
generate(320, 200)
generate(480, 300)
generate(640, 400)
generate(800, 500)
generate(1024, 640)
generate(1280, 800)
generate(1440, 900)
generate(1600, 1000)
generate(1680, 1050)
generate(1920, 1200)
generate(2560, 1600)
generate(320, 320)
generate(480, 480)
generate(640, 640)
generate(800, 800)
generate(1024, 1024)