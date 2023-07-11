"""
@file sobel_demo.py
@brief Sample code using Sobel and/or Scharr OpenCV functions to make a simple Edge Detector
"""
import sys
import cv2 as cv
def main(argv):
 
 scale = 1
 delta = 0
 ddepth = cv.CV_16S
 
 if len(argv) < 1:
    print ('Not enough parameters')
    print ('Usage:\nsobel_filter.py < path_to_image >')
    return -1
 # Load the image
 src = cv.imread(argv[0], cv.IMREAD_COLOR)
 # Check if image is loaded fine
 if src is None:
    print ('Error opening image: ' + argv[0])
    return -1

 grad_x = cv.Sobel(src, ddepth, 1, 0, ksize=3, scale=scale, delta=delta, borderType=cv.BORDER_DEFAULT)
 # Gradient-Y
 # grad_y = cv.Scharr(gray,ddepth,0,1)
 grad_y = cv.Sobel(src, ddepth, 0, 1, ksize=3, scale=scale, delta=delta, borderType=cv.BORDER_DEFAULT)
 
 
 abs_grad_x = cv.convertScaleAbs(grad_x)
 abs_grad_y = cv.convertScaleAbs(grad_y)
 
 
 grad = cv.addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0)

 cv.imwrite(argv[0][:-4]+"_sobel.png", grad)

 return 0

if __name__ == "__main__":
 main(sys.argv[1:])
