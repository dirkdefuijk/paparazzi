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

#include "modules/core/abi.h"

int opencv_example(char *img, int width, int height)
{ 
  // Watershed-----------------------------------------------------
  // Adapted from:
  // https://docs.opencv.org/3.4/d7/d1c/tutorial_js_watershed.html
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

  // grayscale_opencv_to_yuv422(background, img, width, height); //img updated

  // Sectioning------------------------------------------------------
  
  // Mat image(height, width, CV_8UC2, img); // convert back to mat  
  // ret,thresh1 = cv2.threshold(img,127,255,cv2.THRESH_BINARY)
  // Mat image = background; 

  // Partition in 8 vertical section --------------------------------
  static Mat img_sections[8];
  static int N = 8;
  int height_cutoff = height / N;

  // Find collisions -------------------------------------------
  double ratio_best = 1.0; // best ratio in a given section, number is section
  int sections[N] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t best_index = 9;
  int x = width;
  int y = 0;
  // int w = 0;
  int h = height_cutoff;
  
  for (size_t i = 0; i < N; i++)
  {
    Mat img_sections = background( Range(y, y+h-1),Range(0,x)  ); //section horizontal, which is the width when rotated
    // Mat img_sections = image( Range(y, y+h-1),Range(0,x)  ); //section horizontal, which is the width when rotated
    // Mat1b binary;
    // https://stackoverflow.com/questions/31231565/countnonzero-function-gives-an-assertion-error-in-opencv
    // threshold(img_sections, binary, 1, 1, THRESH_BINARY); 
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

  AbiSendMsgWATERSHED_BEST(1, best_index);
}
