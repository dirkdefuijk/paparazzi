/*
 * Copyright (C) 2019 Kirk Scheper <kirkscheper@gmail.com>
 *
 * This file is part of Paparazzi.
 *
 * Paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * Paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * @file modules/computer_vision/cv_detect_object.h
 * Assumes the object consists of a continuous color and checks
 * if you are over the defined object or not
 */

// Own header
#include "modules/computer_vision/cv_detect_color_object.hpp"
#include "modules/computer_vision/cv.h"
#include "modules/core/abi.h"
#include "std.h"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "pthread.h"

using namespace std;
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
#include "opencv_image_functions.h"

#define PRINT(string,...) fprintf(stderr, "[object_detector->%s()] " string,__FUNCTION__ , ##__VA_ARGS__)
#if OBJECT_DETECTOR_VERBOSE
#define VERBOSE_PRINT PRINT
#else
#define VERBOSE_PRINT(...)
#endif

static pthread_mutex_t mutex;
#ifndef COLOR_OBJECT_DETECTOR_FPS1
#define COLOR_OBJECT_DETECTOR_FPS1 0 ///< Default FPS (zero means run at camera fps)
#endif
#ifndef COLOR_OBJECT_DETECTOR_FPS2
#define COLOR_OBJECT_DETECTOR_FPS2 0 ///< Default FPS (zero means run at camera fps)
#endif

// TODO: Change description
/*
 * find_object_centroid
 *
 * Finds the centroid of pixels in an image within filter bounds.
 * Also returns the amount of pixels that satisfy these filter bounds.
 *
 * @param img - input image to process formatted as YUV422.
 * @param p_xc - x coordinate of the centroid of color object
 * @param p_yc - y coordinate of the centroid of color object
 * @param lum_min - minimum y value for the filter in YCbCr colorspace
 * @param lum_max - maximum y value for the filter in YCbCr colorspace
 * @param cb_min - minimum cb value for the filter in YCbCr colorspace
 * @param cb_max - maximum cb value for the filter in YCbCr colorspace
 * @param cr_min - minimum cr value for the filter in YCbCr colorspace
 * @param cr_max - maximum cr value for the filter in YCbCr colorspace
 * @param draw - whether or not to draw on image
 * @return number of pixels of image within the filter bounds.
 */
int watershed(char *img, int width, int height)
{
  Mat M(height, width, CV_8UC2, img);
  Mat gray;
  Mat opening;
  Mat background;
  Mat M1 = Mat::ones(3,3,CV_8U);
  cvtColor(M, gray, CV_YUV2GRAY_Y422, 0);
  threshold(gray, gray, 0, 255, THRESH_BINARY_INV+THRESH_OTSU);

  erode(gray,gray,M1);
  dilate(gray, opening, M1);
  dilate(opening, background, M1, Point(-1,-1), 3);

  grayscale_opencv_to_yuv422(background, img, width, height);
  
  // Partition in 8 vertical section ------------------------------
  static Mat img_sections[8];
  static int N = 8;
  Mat image = background; 
  int height_cutoff = height / N;

  // Find collisions -------------------------------------------
  double ratio_best = 1.0; // best ratio in a given section, number is section
  int sections[N] = {0, 0, 0, 0, 0, 0, 0, 0};
  int best_index = 69;

  int x = width;
  int y = 0;
  // int w = 0;
  int h = height_cutoff;
  
  for (size_t i = 0; i < N; i++)
  {
    Mat img_sections = image( Range(y, y+h-1),Range(0,x)  ); //section horizontal, which is the width when rotated
    y = y + h;

    int black = countNonZero(img_sections); // Count the amount of black pixels
    cv::Size s = img_sections.size();
    int pixel_amount = s.width * s.height;
    float ratio = static_cast<double>(black) / pixel_amount;


    if (ratio < 0.5){ // the set threshold for allowed amount of black in an image
      sections[i] = 1;
      if (ratio <= ratio_best) // the equal ensures that good options don't end up at the end or forgotten
      {
        ratio_best = ratio;
        best_index = i;
      }
    }
  }
  //TODO: maybe pthread some stuff?
  // pthread_mutex_lock(&mutex);
  // global_filters[filter-1].color_count = count;
  // global_filters[filter-1].x_c = x_c;
  // global_filters[filter-1].y_c = y_c;
  // global_filters[filter-1].updated = true;
  // pthread_mutex_unlock(&mutex);

  // printf("best_index %i \n",best_index);
  VERBOSE_PRINT("best_index %i",best_index);
  return best_index;
}

void color_object_detector_periodic(void)
{
  //TODO: check this alloc shit
  // int best = NULL;
  // char *img, int width, int height
  // int best = watershed(char *img, int width, int height);
  int width =  240;
  int height = 520;
  Mat imageIN = char *img;
  int best = watershed(imageIN, width, height);
  // static struct color_object_t local_filters[2];
  // pthread_mutex_lock(&mutex);
  // memcpy(local_filters, global_filters, 2*sizeof(struct color_object_t));
  // pthread_mutex_unlock(&mutex);
  AbiSendMsgWATERSHED_SECTIONS(COLOR_OBJECT_DETECTION2_ID, best);
  // if (best!= NULL){
  //   AbiSendMsgWATERSHED_SECTIONS(COLOR_OBJECT_DETECTION2_ID, best);
  // }
}