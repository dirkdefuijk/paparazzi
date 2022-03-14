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
// #include "modules/computer_vision/cv_detect_color_object.h"
#include "modules/computer_vision/cv.h"
#include "modules/core/abi.h"
#include "std.h"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "pthread.h"

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

// Settings
// uint8_t cod_lum_min1 = 0;


// define global variables
struct color_object_t {
  int32_t x_c;
  int32_t y_c;
  uint32_t color_count;
  bool updated;
};
struct color_object_t global_filters[2];

// Functions

Mat watershedder (char *img, int width, int height)
{
  // Create a new image, using the original bebop image.
  Mat M(height, width, CV_8UC2, img);
  Mat image;
  //  Grayscale image example
  cvtColor(M, image, CV_YUV2GRAY_Y422);
  thresh(gray, 0, 255, cv2.THRESH_BINARY_INV + THRESH_OTSU);
  // Noise removal
  kernel = kernel_create(char *img, int width, int height, uint32_t);
  closing = morphologyEx(thresh, MORPH_CLOSE,kernel, iterations = 2 );
  // background area
  sure_bg = dilate(closing, kernel, iterations = 3);
  // Threshold
  // TODO: Check multiplation result and other math results
  sure_fg = threshold(dist_transform, 0.1 * dist_transform.max(), 255, 0);
  // sure_fg_rot = ndimage.rotate(sure_fg, 90)
  // img_gray_rot = ndimage.rotate(gray, 90)
  return sure_fg
} 

Mat[9] partition_9_vertical(Mat image){
  static Mat img_sections[9];
  static int N = 9;
  height = (int) sizeof(image[0]);
  width  = (int) sizeof(image[1]);
  width_cutoff = width / N;

  for (size_t i = 0; i < N; i++)
  {
    if (i==0)
    {
      sections[0] = img[:, :width_cutoff*i] 
    }else{
      sections[i] = img[width_cutoff*(i-1), :width_cutoff*i] 
    }
  }
  return img_sections
}

// int collisions(Mat[9] img_sections, double* ratio, double* ratio_best){
int collisions(Mat[9] img_sections){
  // int white = 0     // collision pixels
  // int black = 0 // free pixel
  struct pixels{
    int white = 0;
    int black = 0;  };
  int size = 0 // size of the section
  int sections[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int best_index = NULL;
  
  for (size_t i = 0; i < 9; i++)
  {
    counter_fast(sections[i])
    size = (int) sizeof(section);
    ratio[i] = black / size; 
    if (ratio[i] < 0.3){ // the set threshold for allowed amount of black in an image
      sections[i] = 1;
      // Only matters if section would be collision free
      if (ratio[i] <= ratio_best) // the equal ensures that good options don't end up at the end or forgotten
      {
        ratio_best = ratio[i];
        best_index = i;
      }
    }
  }
  return best_index;
}
//Function to count the colour of the pixels, either black or white
// void counter(Mat section, int* black, int* white){
void counter(Mat section,struct int* pixels){
  size = (int) sizeof(section);
  int count = 0;
  printf("size of the section: %f", size);
  // height = size[0]
  // width  = size[1]

  for(int i=0;i<size;i++){   
    if(section[i]==255)
      count++;
        // *black++;
  }
  *pixels.black = count;
  *pixels.white = size - count;
  // *white = size - *black;
  return 0
}
// Algo speed should not be faster, but often multiple is faster than one
// Check section type and filetype Mat or convert?
void counter_fast(Mat section, int* black, int* white){
  size = (int) sizeof(section);
  int count = 0
  for(int i=0;i<size;i+=5){  // notice the increment in i here...
      if(arr[i] == n)
        count++;
      /* check the next four indexes as well as if arr[i] is the last element of the array */ 
      else if( arr[i+1] == n && i+1 < size)
        count++;
      else if (arr[i + 2] == n && i + 2 < size)
        count++;
      else if(arr[i+3] == n && i+3 < size)
        count++;
      else if(arr[i+4] == n && i+4 < size)
        count++;
    }
    *black = count;
    *white = size - black;
    return 0
}

int run_water(){
  // TODO: check if typedef needed 
  // double ratio[9] = 1.0 //relative amount what is an obstacle of the section
  // double ratio_best = 1.0 //best ratio
  Mat image_watershed;
  int best_section = NULL;
  image_watershed = watershedder();
  best_section = collisions(image_watershed);
  // TODO: Calculate pixels in that section 
  // TODO: Calculate midpoint in that section 
  // TODO: Calculate width and height of that section 
  // TODO: create section list to share in message
  if (best_section == NULL)  {
    printf("shit gone wrong: %i", best_section);  }

  return best_section
}

//-----------------------------------------------------------------------------------------
/*
 * Background detector
 * @param img - input image to process
 * @param filter - which detection filter to process
 * @return img
 */
static int best_section *object_detector(struct image_t *img, uint8_t filter)
{
  //new main --W
  int best_section = test_water();
  return best_section;
}


struct image_t *object_detector1(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector1(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
  return object_detector(img, 1);
}

struct image_t *object_detector2(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector2(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
  return object_detector(img, 2);
}

void color_object_detector_init(void)
{
  memset(global_filters, 0, 2*sizeof(struct color_object_t));
  pthread_mutex_init(&mutex, NULL);
#ifdef COLOR_OBJECT_DETECTOR_CAMERA1
#ifdef COLOR_OBJECT_DETECTOR_LUM_MIN1
  cod_lum_min1 = COLOR_OBJECT_DETECTOR_LUM_MIN1;
  cod_lum_max1 = COLOR_OBJECT_DETECTOR_LUM_MAX1;
  cod_cb_min1 = COLOR_OBJECT_DETECTOR_CB_MIN1;
  cod_cb_max1 = COLOR_OBJECT_DETECTOR_CB_MAX1;
  cod_cr_min1 = COLOR_OBJECT_DETECTOR_CR_MIN1;
  cod_cr_max1 = COLOR_OBJECT_DETECTOR_CR_MAX1;
#endif
#ifdef COLOR_OBJECT_DETECTOR_DRAW1
  cod_draw1 = COLOR_OBJECT_DETECTOR_DRAW1;
#endif

  cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA1, object_detector1, COLOR_OBJECT_DETECTOR_FPS1, 0);
#endif

#ifdef COLOR_OBJECT_DETECTOR_CAMERA2
#ifdef COLOR_OBJECT_DETECTOR_LUM_MIN2
  cod_lum_min2 = COLOR_OBJECT_DETECTOR_LUM_MIN2;
  cod_lum_max2 = COLOR_OBJECT_DETECTOR_LUM_MAX2;
  cod_cb_min2 = COLOR_OBJECT_DETECTOR_CB_MIN2;
  cod_cb_max2 = COLOR_OBJECT_DETECTOR_CB_MAX2;
  cod_cr_min2 = COLOR_OBJECT_DETECTOR_CR_MIN2;
  cod_cr_max2 = COLOR_OBJECT_DETECTOR_CR_MAX2;
#endif
#ifdef COLOR_OBJECT_DETECTOR_DRAW2
  cod_draw2 = COLOR_OBJECT_DETECTOR_DRAW2;
#endif

  cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA2, object_detector2, COLOR_OBJECT_DETECTOR_FPS2, 1);
#endif
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

//TODO: throw watershed in find object centroid and apply same filters
uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw,
                              uint8_t lum_min, uint8_t lum_max,
                              uint8_t cb_min, uint8_t cb_max,
                              uint8_t cr_min, uint8_t cr_max)
{
  uint32_t cnt = 0;
  uint32_t tot_x = 0;
  uint32_t tot_y = 0;
  uint8_t *buffer = img->buf;

  // Go through all the pixels
  for (uint16_t y = 0; y < img->h; y++) {
    for (uint16_t x = 0; x < img->w; x ++) {
      // Check if the color is inside the specified values
      uint8_t *yp, *up, *vp;
      if (x % 2 == 0) {
        // Even x
        up = &buffer[y * 2 * img->w + 2 * x];      // U
        yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y1
        vp = &buffer[y * 2 * img->w + 2 * x + 2];  // V
        //yp = &buffer[y * 2 * img->w + 2 * x + 3]; // Y2
      } else {
        // Uneven x
        up = &buffer[y * 2 * img->w + 2 * x - 2];  // U
        //yp = &buffer[y * 2 * img->w + 2 * x - 1]; // Y1
        vp = &buffer[y * 2 * img->w + 2 * x];      // V
        yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y2
      }
      if ( (*yp >= lum_min) && (*yp <= lum_max) &&
           (*up >= cb_min ) && (*up <= cb_max ) &&
           (*vp >= cr_min ) && (*vp <= cr_max )) {
        cnt ++;
        tot_x += x;
        tot_y += y;
        if (draw){
          *yp = 255;  // make pixel brighter in image
        }
      }
    }
  }
  if (cnt > 0) {
    *p_xc = (int32_t)roundf(tot_x / ((float) cnt) - img->w * 0.5f);
    *p_yc = (int32_t)roundf(img->h * 0.5f - tot_y / ((float) cnt));
  } else {
    *p_xc = 0;
    *p_yc = 0;
  }
  return cnt;
}
void color_object_detector_periodic(void)
{
  //TODO: check this alloc shit
  static struct color_object_t local_filters[2];
  pthread_mutex_lock(&mutex);
  memcpy(local_filters, global_filters, 2*sizeof(struct color_object_t));
  pthread_mutex_unlock(&mutex);
  if (best!= NULL){
    AbiSendMsgWATERSHED_SECTIONS(COLOR_OBJECT_DETECTION2_ID, best_section);
  }
}
