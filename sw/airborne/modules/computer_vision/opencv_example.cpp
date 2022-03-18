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
  Mat image = M1;
  int width_cutoff = width / N;

  // Find collisions -------------------------------------------
  int size = 0; // size of the section
  int ratio_best = 1; // best ratio in a given section, number is section
  int sections[N] = {0, 0, 0, 0, 0, 0, 0, 0};
  int ratio[N] = {0, 0, 0, 0, 0, 0, 0, 0};
  int best_index = 69;

  int y = height;
  int h = 0;
  int w = width_cutoff;
  int x = 0;
  for (size_t i = 0; i < N; i++)
  {
    x = w*i;
    Mat img_sections = image( Range(0,y), Range(x,x+w) );
    int black = countNonZero(img_sections); // Count the amount of black pixels
    int size_0 = img_sections.size[0];
    int size_1 = img_sections.size[1];
    int pixel_amount = size_0 * size_1;
    

    ratio[i] = black / pixel_amount; 
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
  printf("%i",best_index);
  return 0;
}
