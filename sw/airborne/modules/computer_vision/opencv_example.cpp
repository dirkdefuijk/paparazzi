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

// New functions -- W
//Function to count the colour of the pixels, either black or white
// void counter(Mat section, int* black, int* white){
void counter(Mat section, struct int *pixels){
  size = section.size[1];// width 
  int count = 0;
  printf("size of the section: %f", size);


  for(int i=0;i<size;i++){   
    if(section[i]==255)
      count++;
        // *black++;
  }
  *pixels.black = count;
  *pixels.white = size - count;
  // *white = size - *black;
  return void;
}
// Algo speed should not be faster, but often multiple is faster than one
// Check section type and filetype Mat or convert?
void counter_fast(Mat section, int *black, int *white){
  size = section.size[1]; // width 
  int count = 0;
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

Mat * partition_9_vertical(Mat image){
  static Mat img_sections[9];
  static int N = 9;
  // int height = (int) sizeof(image[0]);
  int height = image.size[0];
  int width  = image.size[1]; // width

  int width_cutoff = width / N;

  for (size_t i = 0; i < N; i++)
  {
    if (i==0)
    {
      // img_sections[0] = image[, :width_cutoff*i] 
      img_sections[0] = image(cv::Rect(0, 0, width_cutoff*i, height));
      // cv::Mat OutImage = Image(cv::Rect(7,47,1912,980))
    }else{
      // img_sections[i] = image[width_cutoff*(i-1), :width_cutoff*i] 
      img_sections[i] = image(cv::Rect(width_cutoff*i, 0, width_cutoff*(i+1),height));
    }
  }
  return img_sections;
}

// int collisions(Mat[9] img_sections, double* ratio, double* ratio_best){
// int collisions(Mat[9] img_sections){
  int collisions(Mat img_sections){
  // int white = 0     // collision pixels
  // int black = 0 // free pixel
  struct pixels{
    int white = 0;
    int black = 0;  };
  int size = 0; // size of the section
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


int opencv_example(char *img, int width, int height)
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

  return 0;
}
