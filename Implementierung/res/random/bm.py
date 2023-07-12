import random
from PIL import Image

# generate() mit ChatGPT generiert
def generate(w, h):
    img = Image.new("RGB", (w, h))
    pxarr = []
    for _ in range(w * h):
        red = random.randint(0, 255)
        green = random.randint(0, 255)
        blue = random.randint(0, 255)
        pxarr.append((red, green, blue))

    img.putdata(pxarr)
    img.save("random" + str(w) + "x" + str(h) + ".bmp")

print("Dieses Skript generiert BMP-Bilder mit zufälligen Pixeln für jede mögliche Höhen- und Breitenkombination im eingegebenen Intervall.")

width = int(input("Max Breite: "))
height = int(input("Max Höhe: "))

print("Generiere " + str((width-1)*(height-1)) + " Bilder...")

for i in range(1, width+1):
    for j in range (1, height+1):
        generate(i,j)
        print("random" + str(i) + "x" + str(j) + ".bmp")