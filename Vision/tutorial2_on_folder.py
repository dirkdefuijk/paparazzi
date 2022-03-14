# -*- coding: utf-8 -*-
"""
Created on Thu Mar  3 17:25:07 2022

@author: WK
"""

import numpy as np
import cv2
from matplotlib import pyplot as plt
import os
from scipy import ndimage

def watershedder2(img):
    # https://www.bogotobogo.com/python/OpenCV_Python/python_opencv3_Image_Watershed_Algorithm_Marker_Based_Segmentation_2.php
    # img = cv2.imread('image1.jpg')
    b,g,r = cv2.split(img)
    rgb_img = cv2.merge([r,g,b])
    
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    ret, thresh = cv2.threshold(gray,0,255,cv2.THRESH_BINARY_INV+cv2.THRESH_OTSU)
    
    # noise removal
    kernel = np.ones((2,2),np.uint8)
    #opening = cv2.morphologyEx(thresh,cv2.MORPH_OPEN,kernel, iterations = 2)
    closing = cv2.morphologyEx(thresh,cv2.MORPH_CLOSE,kernel, iterations = 2)
    
    # sure background area
    sure_bg = cv2.dilate(closing,kernel,iterations=3)
    
    # Finding sure foreground area
    dist_transform = cv2.distanceTransform(sure_bg,cv2.DIST_L2,3)
    
    # Threshold
    ret, sure_fg = cv2.threshold(dist_transform,0.1*dist_transform.max(),255,0)
    
    # Finding unknown region
    sure_fg = np.uint8(sure_fg)
    unknown = cv2.subtract(sure_bg,sure_fg)
    
    # Marker labelling
    ret, markers = cv2.connectedComponents(sure_fg)
    # sure_fg_rot = ndimage.rotate(sure_fg, 90)
    # gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    
    # # Add one to all labels so that sure background is not 0, but 1
    # markers = markers+1
    
    # Now, mark the region of unknown with zero
    markers[unknown==255] = 0
    
    markers = cv2.watershed(img,markers)
    img[markers == -1] = [255,0,0]
    
    img_gray_rot = ndimage.rotate(gray, 90)
    img_rot = ndimage.rotate(img, 90)
    img_unkown_rot = ndimage.rotate(unknown, 90)
    cv2.imshow('gray rot', img_gray_rot)
    cv2.imshow('watershed', img_rot)
    cv2.imshow('unknown', img_unkown_rot)
    
    
    # plt.subplot(421),plt.imshow(rgb_img)
    # plt.title('Input Image'), plt.xticks([]), plt.yticks([])
    # plt.subplot(422),plt.imshow(thresh, 'gray')
    # plt.title("Otsu's binary threshold"), plt.xticks([]), plt.yticks([])
    
    # plt.subplot(423),plt.imshow(closing, 'gray')
    # plt.title("morphologyEx:Closing:2x2"), plt.xticks([]), plt.yticks([])
    # plt.subplot(424),plt.imshow(sure_bg, 'gray')
    # plt.title("Dilation"), plt.xticks([]), plt.yticks([])
    
    # plt.subplot(425),plt.imshow(dist_transform, 'gray')
    # plt.title("Distance Transform"), plt.xticks([]), plt.yticks([])
    # plt.subplot(426),plt.imshow(sure_fg, 'gray')
    # plt.title("Thresholding"), plt.xticks([]), plt.yticks([])
    
    # plt.subplot(427),plt.imshow(unknown, 'gray')
    # plt.title("Unknown"), plt.xticks([]), plt.yticks([])
    
    # plt.subplot(428),plt.imshow(img, 'gray')
    # plt.title("Result from Watershed"), plt.xticks([]), plt.yticks([])
    
    # plt.tight_layout()
    # plt.show()


# Main loop
path =  os.path.join(os.getcwd(),"AE4317_2019_datasets/cyberzoo_aggressive_flight/20190121-144646")

print(path)
for imagePath in os.listdir(path):
    
    input_path = os.path.join(path, imagePath)
    # print(input_path)
    
    img = cv2.imread(input_path)
    
    rotated = ndimage.rotate(img, 90)
    img_gray = cv2.cvtColor(rotated, cv2.COLOR_BGR2GRAY)
    # window_name = 'image_in'
    # cv2.imshow(window_name, img)    
    # print(  np.shape(img_gray))
    watershedder2(img)

    key = cv2.waitKey(50)
    
    if key==81 or key==113:
        break
    
    
