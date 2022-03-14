# -*- coding: utf-8 -*-
"""
Created on Thu Mar  3 14:33:50 2022

@author: Pim de Ruijter
"""

import numpy as np
import cv2 as cv
from matplotlib import pyplot as plt
# https://docs.opencv.org/4.x/d3/db4/tutorial_py_watershed.html
import os
# assign directory
# 'C:/Users/Pim de Ruijter/Documents/Studie/AE4317/Python/testbatch_hard'
directory = os.getcwd()
path =  os.path.join(os.getcwd(),"AE4317_2019_datasets/cyberzoo_aggressive_flight/20190121-144646") 
print(path)
directory = path
 
# iterate over files in
# that directory
for filename in os.listdir(directory):
    f = os.path.join(directory, filename)
    # checking if it is a file
    if os.path.isfile(f):
        
        img = cv.imread(f)
        # norm = np.zeros((80,80))
        # img = cv.normalize(img,  norm, 0, 255, cv.NORM_MINMAX)
        
        gray = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
        ret, thresh = cv.threshold(gray,0,255,cv.THRESH_BINARY_INV+cv.THRESH_OTSU)
        
        # noise removal
        kernel = np.ones((3,3),np.uint8)
        opening = cv.morphologyEx(thresh,cv.MORPH_OPEN,kernel, iterations = 1)
        # sure background area
        sure_bg = cv.dilate(opening,kernel,iterations=2)
        plt.imshow(sure_bg)
        plt.show()


#img = cv.imread('coins.jpeg')
print(f)
img = cv.imread(f)
#img = cv.imread('im7.jpg')

#plt.imshow(img2)
# plt.show()

gray = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
ret, thresh = cv.threshold(gray,0,255,cv.THRESH_BINARY_INV+cv.THRESH_OTSU)

# noise removal
kernel = np.ones((3,3),np.uint8)
opening = cv.morphologyEx(thresh,cv.MORPH_OPEN,kernel, iterations = 3)
# sure background area
sure_bg = cv.dilate(opening,kernel,iterations=1)
# # Finding sure foreground area
# dist_transform = cv.distanceTransform(opening,cv.DIST_L2,5)
# ret, sure_fg = cv.threshold(dist_transform,0.7*dist_transform.max(),255,0)
# # Finding unknown region
# sure_fg = np.uint8(sure_fg)
# unknown = cv.subtract(sure_bg,sure_fg)

# # Marker labelling
# ret, markers = cv.connectedComponents(sure_fg)
# # Add one to all labels so that sure background is not 0, but 1
# markers = markers+1
# # Now, mark the region of unknown with zero
# markers[unknown==255] = 0


# markers = cv.watershed(img,markers)
# img[markers == -1] = [255,0,0]


plt.imshow(sure_bg)
plt.show()

# plt.imshow(markers)
# plt.show()
# plt.imshow(img)
# plt.show()

