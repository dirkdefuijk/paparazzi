import cv2
import os
from scipy import ndimage
import numpy as np

# Read the original image

#img = cv2.imread('Swan_In_Pond_Background-411.jpg')

#cap = cv2.VideoCapture("ba79fcaa-419b-485e-8fbb-aed4c25f86b7.mp4")
#
#while(cap.isOpened()):
#    ret, img = cap.read()  
#    
#    if ret == True:
#        img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
#        img_blur = cv2.GaussianBlur(img_gray, (3,3), 0)
#        edges = cv2.Canny(image=img_blur, threshold1=100, threshold2=200)
#        cv2.imshow('Canny Edge Detection', edges)
#     
#    key = cv2.waitKey(50)
#    
#    if key==81 or key==113:
#        break
#
#cap.release()

# path =  os.path.join( os.getcwd(),"AE4317_2019_datasets\\cyberzoo_canvas_approach\\20190121-151448")
# path =  os.path.join(os.getcwd(),"AE4317_2019_datasets\\cyberzoo_poles_panels\\20190121-140205")
path =  os.path.join(os.getcwd(),"AE4317_2019_datasets\\cyberzoo_aggressive_flight\\20190121-144646")


for imagePath in os.listdir(path):
    
    input_path = os.path.join(path, imagePath)
    img = cv2.imread(input_path)
    rotated = ndimage.rotate(img, 90)
    
    img_gray = cv2.cvtColor(rotated, cv2.COLOR_BGR2GRAY)
    img_blur = cv2.GaussianBlur(img_gray, (3,3), 0)
    edges = cv2.Canny(image=img_blur, threshold1=30, threshold2=1)
    
    output = np.concatenate((edges, img_gray), axis = 1)
    
    cv2.imshow('Canny Edge Detection', output)

    key = cv2.waitKey(50)
    
    if key==81 or key==113:
        break
    
    
    

    
## Convert to graycsale
#img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
## Blur the image for better edge detection
#img_blur = cv2.GaussianBlur(img_gray, (3,3), 0)
## Sobel Edge Detection
#sobelx = cv2.Sobel(src=img_blur, ddepth=cv2.CV_64F, dx=1, dy=0, ksize=5) # Sobel Edge Detection on the X axis
#sobely = cv2.Sobel(src=img_blur, ddepth=cv2.CV_64F, dx=0, dy=1, ksize=5) # Sobel Edge Detection on the Y axis
#sobelxy = cv2.Sobel(src=img_blur, ddepth=cv2.CV_64F, dx=1, dy=1, ksize=5) # Combined X and Y Sobel Edge Detection
## Canny Edge Detection
#edges = cv2.Canny(image=img_blur, threshold1=100, threshold2=200) # Canny Edge Detection
## Display Canny Edge Detection Image
#cv2.imshow('Canny Edge Detection', edges)
#cv2.waitKey(0)

cv2.destroyAllWindows()