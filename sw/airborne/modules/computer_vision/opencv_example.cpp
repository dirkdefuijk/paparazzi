/*
 * Copyright (C) C. De Wagter
 *
 * This file is part of paparazzi
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * @file "modules/computer_vision/opencv_example.cpp"
 * @author C. De Wagter
 * A simple module showing what you can do with opencv on the bebop.
 */


#include "opencv_example.h"



using namespace std;
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
#include "opencv_image_functions.h"

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
// Function that runs watershed
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
// original function used for the passthrough of variables
int opencv_example(char *img, int width, int height)
{
  int best_section =  run_water(char *img, int width, int height);
  return best_section;
}
