# -*- coding: utf-8 -*-
"""
Created on Thu Mar  3 17:16:53 2022

@author: WK
"""

import numpy as np
import cv2
from matplotlib import pyplot as plt
import os
from scipy import ndimage

def watershedder(img):
    

    # https://www.bogotobogo.com/python/OpenCV_Python/python_opencv3_Image_Watershed_Algorithm_Marker_Based_Segmentation_2.php
    # img = cv2.imread('image1.jpg')
    b,g,r = cv2.split(img)
    # rgb_img = cv2.merge([r,g,b])
    
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    # cv2.imshow("gray", gray)
    
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
    # cv2.imshow("watershed", sure_fg)
    # print(    np.shape(sure_fg))
    # print(  np.shape(img))
    # output = np.concatenate((sure_fg, img), axis = 1)
    # img_gray = cv2.cvtColor(rgb_img, cv2.COLOR_BGR2GRAY)
    # print(  np.shape(img_gray))
    sure_fg_rot = ndimage.rotate(sure_fg, 90)
    img_gray_rot = ndimage.rotate(gray, 90)
    cv2.imshow('gray rot', img_gray_rot)
    cv2.imshow('watershed', sure_fg_rot)
    # img_gray_rot = ndimage.rotate(img_gray, 90)
    # output = np.concatenate((img_gray_rot, sure_fg_rot), axis = 1)
    # cv2.imshow('Watershed', output)
    return sure_fg
    # Optional plots, to visualize watershed
    # plt.subplot(321),plt.imshow(rgb_img)
    # plt.title('Input Image'), plt.xticks([]), plt.yticks([])
    # plt.subplot(322),plt.imshow(thresh, 'gray')
    # plt.title("Otsu's binary threshold"), plt.xticks([]), plt.yticks([])
    
    # plt.subplot(323),plt.imshow(closing, 'gray')
    # plt.title("morphologyEx:Closing:2x2"), plt.xticks([]), plt.yticks([])
    # plt.subplot(324),plt.imshow(sure_bg, 'gray')
    # plt.title("Dilation"), plt.xticks([]), plt.yticks([])
    
    # plt.subplot(325),plt.imshow(dist_transform, 'gray')
    # plt.title("Distance Transform"), plt.xticks([]), plt.yticks([])
    # plt.subplot(326),plt.imshow(sure_fg, 'gray')
    # plt.title("Thresholding"), plt.xticks([]), plt.yticks([])
    
    # plt.tight_layout()
    # plt.show()

# Main loop
# path =  os.path.join(os.getcwd(),"AE4317_2019_datasets/cyberzoo_aggressive_flight/20190121-144646")
path =  os.path.join(os.getcwd(),"Test_runs\\run2")
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
    watershedder(img)

    key = cv2.waitKey(50)
    
    if key==81 or key==113:
        break
    
    

