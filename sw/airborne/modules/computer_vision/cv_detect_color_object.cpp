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

// Filter Settings
uint8_t cod_lum_min1 = 0;
uint8_t cod_lum_max1 = 0;
uint8_t cod_cb_min1 = 0;
uint8_t cod_cb_max1 = 0;
uint8_t cod_cr_min1 = 0;
uint8_t cod_cr_max1 = 0;

uint8_t cod_lum_min2 = 0;
uint8_t cod_lum_max2 = 0;
uint8_t cod_cb_min2 = 0;
uint8_t cod_cb_max2 = 0;
uint8_t cod_cr_min2 = 0;
uint8_t cod_cr_max2 = 0;

bool cod_draw1 = false;
bool cod_draw2 = false;

uint8_t best = 9; // --W

// define global variables
struct color_object_t {
  int32_t x_c;
  int32_t y_c;
  uint32_t color_count;
  bool updated;
};
struct color_object_t global_filters[2];

// Function
uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw,
                              uint8_t lum_min, uint8_t lum_max,
                              uint8_t cb_min, uint8_t cb_max,
                              uint8_t cr_min, uint8_t cr_max);

/*
 * object_detector
 * @param img - input image to process
 * @param filter - which detection filter to process
 * @return img
 */

static struct image_t *object_detector(struct image_t *img, uint8_t filter)
{
  // return img;
  int width =  240;
  int height = 520;
  watershed(img,width,height);
  return img;
}

// struct image_t *object_detector1(struct image_t *img, uint8_t camera_id);
// struct image_t *object_detector1(struct image_t *img, uint8_t camera_id __attribute__((unused)))
struct image_t *object_detector1(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector1(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
  return object_detector(img, 1);
  // int width =  240;
  // int height = 520;
  // return watershed(img,width,height);
}

// // struct image_t *object_detector2(struct image_t *img, uint8_t camera_id);
// struct image_t *object_detector2(struct image_t *img, uint8_t camera_id);
// // struct image_t *object_detector2(struct image_t *img, uint8_t camera_id __attribute__((unused)))
// struct image_t *object_detector2(struct image_t *img, uint8_t camera_id __attribute__((unused)))
// {
//   // return object_detector(img, 2);
//   int width =  240;
//   int height = 520;
//   return watershed(img,width,height);
// }

void color_object_detector_init(void)
{
  // These are probably not needed. Were there just to be sure og code, need maybe if failure
  memset(global_filters, 0, 2*sizeof(struct color_object_t));
  pthread_mutex_init(&mutex, NULL);
  // -- W 
  // cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA2, watershed, COLOR_OBJECT_DETECTOR_FPS2, 1); //--W optiflow_module.c >> similar setup used/needed
  cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA1, object_detector1, COLOR_OBJECT_DETECTOR_FPS2, 1); //--W optiflow_module.c >> similar setup used/needed
  // cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA1, watershed, COLOR_OBJECT_DETECTOR_FPS1, 1); //--W optiflow_module.c >> similar setup used/needed
  // cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA2, object_detector, COLOR_OBJECT_DETECTOR_FPS2, 1); //--W optiflow_module.c >> similar setup used/needed
  // cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA2, watershed, COLOR_OBJECT_DETECTOR_FPS2, 1); //--W optiflow_module.c >> similar setup used/needed
  // not needed but keep to avoid errors
// #ifdef COLOR_OBJECT_DETECTOR_CAMERA1
// #ifdef COLOR_OBJECT_DETECTOR_LUM_MIN1
//   cod_lum_min1 = COLOR_OBJECT_DETECTOR_LUM_MIN1;
//   cod_lum_max1 = COLOR_OBJECT_DETECTOR_LUM_MAX1;
//   cod_cb_min1 = COLOR_OBJECT_DETECTOR_CB_MIN1;
//   cod_cb_max1 = COLOR_OBJECT_DETECTOR_CB_MAX1;
//   cod_cr_min1 = COLOR_OBJECT_DETECTOR_CR_MIN1;
//   cod_cr_max1 = COLOR_OBJECT_DETECTOR_CR_MAX1;
// #endif
// #ifdef COLOR_OBJECT_DETECTOR_DRAW1
//   cod_draw1 = COLOR_OBJECT_DETECTOR_DRAW1;
// #endif

//   cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA1, object_detector1, COLOR_OBJECT_DETECTOR_FPS1, 0);
// #endif

// #ifdef COLOR_OBJECT_DETECTOR_CAMERA2
// #ifdef COLOR_OBJECT_DETECTOR_LUM_MIN2
//   cod_lum_min2 = COLOR_OBJECT_DETECTOR_LUM_MIN2;
//   cod_lum_max2 = COLOR_OBJECT_DETECTOR_LUM_MAX2;
//   cod_cb_min2 = COLOR_OBJECT_DETECTOR_CB_MIN2;
//   cod_cb_max2 = COLOR_OBJECT_DETECTOR_CB_MAX2;
//   cod_cr_min2 = COLOR_OBJECT_DETECTOR_CR_MIN2;
//   cod_cr_max2 = COLOR_OBJECT_DETECTOR_CR_MAX2;
// #endif
// #ifdef COLOR_OBJECT_DETECTOR_DRAW2
//   cod_draw2 = COLOR_OBJECT_DETECTOR_DRAW2;
// #endif

  // cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA2, object_detector2, COLOR_OBJECT_DETECTOR_FPS2, 1);
  // cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA2, watershed, COLOR_OBJECT_DETECTOR_FPS2, 1);
// #endif
}

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
// uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw,
//                               uint8_t lum_min, uint8_t lum_max,
//                               uint8_t cb_min, uint8_t cb_max,
//                               uint8_t cr_min, uint8_t cr_max)
// {
//   uint32_t cnt = 0;
//   uint32_t tot_x = 0;
//   uint32_t tot_y = 0;
//   uint8_t *buffer = img->buf;

//   // Go through all the pixels
//   for (uint16_t y = 0; y < img->h; y++) {
//     for (uint16_t x = 0; x < img->w; x ++) {
//       // Check if the color is inside the specified values
//       uint8_t *yp, *up, *vp;
//       if (x % 2 == 0) {
//         // Even x
//         up = &buffer[y * 2 * img->w + 2 * x];      // U
//         yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y1
//         vp = &buffer[y * 2 * img->w + 2 * x + 2];  // V
//         //yp = &buffer[y * 2 * img->w + 2 * x + 3]; // Y2
//       } else {
//         // Uneven x
//         up = &buffer[y * 2 * img->w + 2 * x - 2];  // U
//         //yp = &buffer[y * 2 * img->w + 2 * x - 1]; // Y1
//         vp = &buffer[y * 2 * img->w + 2 * x];      // V
//         yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y2
//       }
//       if ( (*yp >= lum_min) && (*yp <= lum_max) &&
//            (*up >= cb_min ) && (*up <= cb_max ) &&
//            (*vp >= cr_min ) && (*vp <= cr_max )) {
//         cnt ++;
//         tot_x += x;
//         tot_y += y;
//         if (draw){
//           *yp = 255;  // make pixel brighter in image
//         }
//       }
//     }
//   }
//   if (cnt > 0) {
//     *p_xc = (int32_t)roundf(tot_x / ((float) cnt) - img->w * 0.5f);
//     *p_yc = (int32_t)roundf(img->h * 0.5f - tot_y / ((float) cnt));
//   } else {
//     *p_xc = 0;
//     *p_yc = 0;
//   }
//   return cnt;
// }



// int watershed(char *img, int width, int height)

// static struct image_t *watershed(struct image_t *img, int width, int height)
// struct image_t watershed(struct image_t *img, int width, int height)
// struct image_t watershed(struct image_t *img, int width, int height)
void watershed(struct image_t *img, int width, int height)
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

  // grayscale_opencv_to_yuv422(background, (char *)  img->buf, width, height); //--W detect contour same type conv.
  grayscale_opencv_to_yuv422( background, img, width, height); //--W detect contour same type conv.grayscale_opencv_to_yuv422(background, (char *) img->buf, width, height); //--W detect contour same type conv.

  
  // return img;
  // return NULL;
}
uint_8 best_index best_section(void){
  Mat M(height, width, CV_8UC2, img); // convert back to mat
  // Partition in 8 vertical section ------------------------------
  static Mat img_sections[8];
  static int N = 8;
  // Mat image = background; 
  Mat image = M;
  int height_cutoff = height / N;

  // Find collisions -------------------------------------------
  double ratio_best = 1.0; // best ratio in a given section, number is section
  int sections[N] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint_8 best_index = 9;

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
  uint_8 best = best_section(img);
  // uint_8 best = 0;
  //TODO: check this alloc shit
  // TODO: get from images
  // int best = NULL;
  // char *img, int width, int height
  // int best = watershed(char *img, int width, int height);
  // int width =  240;
  // int height = 520;
  // Mat imageIN = char *img;
  // int best = watershed(imageIN, width, height);
  // int best = watershed(struct image_t *img, width, height);
  // int best = watershed(img, width, height);
  // static struct color_object_t local_filters[2];
  // pthread_mutex_lock(&mutex);
  // memcpy(local_filters, global_filters, 2*sizeof(struct color_object_t));
  // pthread_mutex_unlock(&mutex);
  // AbiSendMsgWATERSHED_BEST(COLOR_OBJECT_DETECTION3_ID, best);
  AbiSendMsgWATERSHED_BEST(COLOR_OBJECT_DETECTION2_ID, best);
  // AbiSendMsgWATERSHED_SECTIONS(COLOR_OBJECT_DETECTION2_ID, best);
  // if (best!= NULL){
  //   AbiSendMsgWATERSHED_SECTIONS(COLOR_OBJECT_DETECTION2_ID, best);
  }
// TA file: mateksys_3901_l0x.c