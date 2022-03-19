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
  
  // Partition in 8 vertical section ------------------------------
  static Mat img_sections[8];
  static int N = 8;
  Mat image = background; 
  int height_cutoff = height / N;

  // Find collisions -------------------------------------------
  // int size = 0; // size of the section
  double ratio_best = 1.0; // best ratio in a given section, number is section
  int sections[N] = {0, 0, 0, 0, 0, 0, 0, 0};
  // float ratio[N] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  // double ratio = 0.0;
  int best_index = 69;
  
  // int width_cutoff=height_cutoff; // bonus content
  // int x = width;
  // int y = height;
  // int h = 0;
  // int w = 1;//width_cutoff;
  
  int x = width;
  int y = 0;
  // int w = 0;
  int h = height_cutoff;
  
  for (size_t i = 0; i < N; i++)
  {
    Mat img_sections = image( Range(y, y+h-1),Range(0,x)  ); //section horizontal, which is the width when rotated
    // Mat img_sections = image( Range(0,x), Range(y, y+h-1) ); //section horizontal, which is the width when rotated
    // Mat img_sections = image( Range(0,y), Range(x,x+w)  ); //section vertical
    y = y + h;
    
    int black = countNonZero(img_sections); // Count the amount of black pixels
    // double black = countNonZero(img_sections); // Count the amount of black pixels
    // printf("black %i \n", black);
    // double size_0 = img_sections.size[0];
    // double size_1 = img_sections.size[1];
    // cv::Size s = img_sections.size();
    // double rows = s.height;
    // double cols = s.width;
    // cv:Mat img_sections;
    // int rows = img_sections.rows;
    // int cols = img_sections.cols;

    cv::Size s = img_sections.size();
    // rows = s.height;
    // cols = s.width;
    // int pixel_amount = size_0 * size_1;
    // double pixel_amount = cols * rows;
    // float pixel_amount = cols * rows;
    int pixel_amount = s.width * s.height;
    // int pixel_amount = cols * rows;
    // printf("rows %d \n", rows);
    // printf("cols %d \n", cols);
    // printf("rows %d \n", s.width);
    // printf("cols %d \n", s.height);
    // printf("pixel_amount %i \n",pixel_amount);
    
    // ratio[i] = black / pixel_amount;
    // ratio = static_cast<double>(black)/static_cast<double>(pixel_amount);
    // ratio = static_cast<float>(black)/static_cast<float>(pixel_amount);
    float ratio = static_cast<double>(black) / pixel_amount;
    // ratio = black / pixel_amount;
    // ratio = black / pixel_amount;
    // printf("black %d \n", black);
    // printf("pixel_amount %d \n", pixel_amount);
    // printf("ratio %f \n\n", ratio);

    if (ratio < 0.5){ // the set threshold for allowed amount of black in an image
    // if (ratio[i] < 0.3){ // the set threshold for allowed amount of black in an image
      sections[i] = 1;
      // Only matters if section would be collision free
      // if (ratio[i] <= ratio_best) // the equal ensures that good options don't end up at the end or forgotten
      if (ratio <= ratio_best) // the equal ensures that good options don't end up at the end or forgotten
      {
        // ratio_best = ratio[i];
        ratio_best = ratio;
        // printf("ratio best %i \n",ratio_best);
        best_index = i;
      }
    }
  }
  printf("best_index %i \n",best_index);
  // VERBOSE_PRINT("best_index %i",best_index);
  return 0;
}

