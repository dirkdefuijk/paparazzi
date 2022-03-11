# -*- coding: utf-8 -*-
"""
Created on Mon Mar  7 12:32:50 2022

@author: WK
"""

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

def watershedder(img,visuals):
    

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
    if visuals:
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

def partition4(img):
    # https://oleksandrg.medium.com/how-to-divide-the-image-into-4-parts-using-opencv-c0afb5cab10c
    # import cv2
    # # load image
    # img = cv2.imread('image_test.jpg')
    ##########################################
    # At first vertical devide image         #
    ##########################################
    # start vertical devide image
    height = img.shape[0]
    width = img.shape[1]
    # Cut the image in half
    width_cutoff = width // 2
    left1 = img[:, :width_cutoff]
    right1 = img[:, width_cutoff:]
    # finish vertical devide image
    ##########################################
    # At first Horizontal devide left1 image #
    ##########################################
    #rotate image LEFT1 to 90 CLOCKWISE
    img = cv2.rotate(left1, cv2.ROTATE_90_CLOCKWISE)
    # start vertical devide image
    height = img.shape[0]
    width = img.shape[1]
    # Cut the image in half
    width_cutoff = width // 2
    l1 = img[:, :width_cutoff]
    l2 = img[:, width_cutoff:]
    # finish vertical devide image
    #rotate image to 90 COUNTERCLOCKWISE
    l1 = cv2.rotate(l1, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # #save
    # cv2.imwrite("one_horisont_1.jpg", l1)
    # #rotate image to 90 COUNTERCLOCKWISE
    # l2 = cv2.rotate(l2, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # #save
    # cv2.imwrite("one_horisont_2.jpg", l2)
    ##########################################
    # At first Horizontal devide right1 image#
    ##########################################
    #rotate image RIGHT1 to 90 CLOCKWISE
    img = cv2.rotate(right1, cv2.ROTATE_90_CLOCKWISE)
    # start vertical devide image
    height = img.shape[0]
    width = img.shape[1]
    # Cut the image in half
    width_cutoff = width // 2
    r1 = img[:, :width_cutoff]
    r2 = img[:, width_cutoff:]
    # finish vertical devide image
    #rotate image to 90 COUNTERCLOCKWISE
    r1 = cv2.rotate(r1, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # #save
    # cv2.imwrite("second_vhorisont_1.jpg", r1)
    # #rotate image to 90 COUNTERCLOCKWISE
    # r2 = cv2.rotate(r2, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # #save
    # cv2.imwrite("second_horisont_2.jpg", r2)
    
    return l1,l2,r1,r2

def partition7(img):
    # https://oleksandrg.medium.com/how-to-divide-the-image-into-4-parts-using-opencv-c0afb5cab10c
    # import cv2
    # # load image
    # img = cv2.imread('image_test.jpg')
    ##########################################
    # At first vertical devide image         #
    ##########################################
    # start vertical devide image
    height = img.shape[0]
    width = img.shape[1]
    # Cut the image in half
    width_cutoff = width // 3
    left1 = img[:, :width_cutoff]
    mid1 = img[:, width_cutoff:(2*width_cutoff)]
    right1 = img[:, (2*width_cutoff):]
    # finish vertical devide image
    ##########################################
    # At first Horizontal devide left1 image #
    ##########################################
    #rotate image LEFT1 to 90 CLOCKWISE
    img = cv2.rotate(left1, cv2.ROTATE_90_CLOCKWISE)
    # start vertical devide image
    # height = img.shape[0]
    width = img.shape[1]
    # Cut the image in half
    width_cutoff = width // 2
    l1 = img[:, :width_cutoff]
    l2 = img[:, width_cutoff:]
    # finish vertical devide image
    #rotate image to 90 COUNTERCLOCKWISE
    l1 = cv2.rotate(l1, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # #save
    # cv2.imwrite("one_horisont_1.jpg", l1)
    # #rotate image to 90 COUNTERCLOCKWISE
    # l2 = cv2.rotate(l2, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # #save
    # cv2.imwrite("one_horisont_2.jpg", l2)
    ##########################################
    # At first Horizontal devide right1 image#
    ##########################################
    #rotate image RIGHT1 to 90 CLOCKWISE
    img = cv2.rotate(right1, cv2.ROTATE_90_CLOCKWISE)
    # start vertical devide image
    # height = img.shape[0]
    width = img.shape[1]
    # Cut the image in half
    width_cutoff = width // 2
    r1 = img[:, :width_cutoff]
    r2 = img[:, width_cutoff:]
    # finish vertical devide image
    #rotate image to 90 COUNTERCLOCKWISE
    r1 = cv2.rotate(r1, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # #save
    # cv2.imwrite("second_vhorisont_1.jpg", r1)
    # #rotate image to 90 COUNTERCLOCKWISE
    # r2 = cv2.rotate(r2, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # #save
    # cv2.imwrite("second_horisont_2.jpg", r2)
    
    # Partition for mid
    img = cv2.rotate(right1, cv2.ROTATE_90_CLOCKWISE)
    # height = img.shape[0]
    width = img.shape[1]
    width_cutoff = width // 3
    m1 = img[:, :width_cutoff]
    m2 = img[:, width_cutoff:(2*width_cutoff)]
    m3 = img[:, (2*width_cutoff):]

    sections = [l1,l2,m1,m2,m3,r1,r2]
    return sections

def collisions_checker(sections):
    # collision_found = True
    # safeWindow= np.zeros([1,7])     #[0,0,0,0,0,0,0]
    # i=0
    # for section in sections: #TODO: index from list
    free =[]
    ratio_best=9E9
    for i in range(0,7):
        # safeWindow[i] = noCollision(section)
        # i=i+1
        safeWindow, ratio = noCollision(sections[i])
        if safeWindow:
            free.append(i)
    # free = (safeWindow==1) # free area to fly to
        if ratio < ratio_best:
            best = i 
    return free, best 

def noCollision(section):
    safeWindow = 0 #default is collsion
    size_section = np.shape(section)
    size_total = size_section[0]*size_section[1]
    white = np.count_nonzero(section == 255) #number of collisions in a section
    black = np.count_nonzero(section == 0) #number of collisions in a section
    print("black white")
    print(black)
    print(white)
    print(size_total)
    # ratio = (black/white)
    ratio = (black/size_total)
    
    print("ratio")
    print(ratio)
    if ratio < 0.3: # more white less like to collide
        safeWindow = 1 # safe to fly to this section
    return safeWindow,ratio

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
    visuals= True #False
    img_wsh = watershedder(img,visuals)
    
    sections = partition7(img_wsh)
    free,best = collisions_checker(sections)
    print("best")
    print(best)
    print(free)
    key = cv2.waitKey(50)
    
    if key==81 or key==113:
        break
    
    

