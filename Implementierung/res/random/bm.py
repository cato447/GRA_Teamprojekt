import random
import subprocess
import os
from PIL import Image

print("Dieses Skript testet BMP-Bilder mit zufälligen Pixeln für jede mögliche Höhen- und Breitenkombination im eingegebenen Intervall.")

width = int(input("Max Breite: "))
height = int(input("Max Höhe: "))

pixel = [(0, 0, 0)] * width * height

for i in range(0, width * height):
        pixel[i] = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

# generate() mit ChatGPT generiert
def generate(w, h):
    global pixel
    img = Image.new("RGB", (w, h))
    img.putdata(pixel[:w*h])
    img.save("random" + str(w) + "x" + str(h) + ".bmp")

print("Generiere " + str((width-1)*(height-1)) + " Bilder...")

for i in range(1, width+1):
    for j in range (1, height+1):
        print(f"Teste: {i} x {j}      ", end='\r')
        generate(i,j)
        name = "random" + str(i) + "x" + str(j) + ".bmp"
        subprocess.run(["../../build/program", "-t", "-V", "1", "-o", "temp.bmp", name])
        try:
            os.remove(name)
        except:
            print("failed to delete " + name)