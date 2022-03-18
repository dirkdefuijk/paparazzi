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
  // int height = (int) sizeof(image[0]);
  // int height = image.size[0];
  // int width  = image.size[1]; // width
  Mat image = M1;
  int width_cutoff = width / N;

  // for (size_t i = 0; i < N; i++)
  // {
    // int y = height-1;
    // int h = 0;
    // int w = width_cutoff-1;
    // int x = w*i-1;
    // img_sections[i] = image( Range(1,y), Range(x,x+w-1) );

    // img_sections[i] = image(cv::Rect(width_cutoff*i, 0, width_cutoff-1, height));
    // img_sections[i] = image(cv::Rect(width_cutoff, 0, width_cutoff, height)); // Dummy --W
  // }
  // int y = 520-1;
  // int x = 240-1;
  // int w = 30-1;
  // img_sections[0] = image( Range(1,y), Range(1,w) );

  // int imgheight = img.rows;
	// int imgwidth = img.cols;
  // x = imgwidth - 1;
  // y = imgheight - 1;
  // x1 = imgwidth - 1;
  // y1 = imgheight - 1;
  // // crop the patches of size MxN
  // Mat tiles = image_copy(Range(y, imgheight), Range(x, imgwidth));
  // rectangle(image, Point(x,y), Point(x1,y1), Scalar(0,255,0), 1);

  // int X = 240;
  // int Y = 520;
  // int W = 30;

  for (size_t i = 0; i < N; i++)
  {
    // img_sections[i] = image(Rect(X,Y,W,H);

    // image(cv::Rect(xMin,yMin,xMax-xMin,yMax-yMin)).copyTo(img_sections[i]);
    
    // Mat cropped_image = background(Range(10,20), Range(100,200));

    int y = height;
    int h = 0;
    int w = width_cutoff;
    int x = w*i;
    Mat img_sections[i] = image( Range(0,y), Range(x,x+w) );
  }
  

  

  // img_sections[1] = image( Range(1,y), Range(w  ,w*2) );
  // img_sections[2] = image( Range(1,y), Range(w*2,w*3) );
  // img_sections[3] = image( Range(1,y), Range(w*3,w*4) );
  // img_sections[4] = image( Range(1,y), Range(w*4,w*5) );
  // img_sections[5] = image( Range(1,y), Range(w*5,w*6) );
  // img_sections[6] = image( Range(1,y), Range(w*6,w*7) );
  // img_sections[7] = image( Range(1,y), Range(w*7,x) );
  
  
  // Find collisions -------------------------------------------
  int size = 0; // size of the section
  int ratio_best = 1; // best ratio in a given section, number is section
  int sections[N] = {0, 0, 0, 0, 0, 0, 0, 0};
  int ratio[N] = {0, 0, 0, 0, 0, 0, 0, 0};
  int best_index = 69;

  for (size_t i = 0; i < N; i++)
  {
    int black = countNonZero(img_sections[i]); // Count the amount of black pixels
    // int black = counter(img_sections[i]);
    // int black = counter_fast(img_sections[i]);
    int size_0 = img_sections[i].size[0];
    int size_1 = img_sections[i].size[1];
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
