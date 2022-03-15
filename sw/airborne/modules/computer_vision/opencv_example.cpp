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
  Mat dst;
  Mat img_blur;
  Mat dilation_dst;

  cvtColor(M, M, CV_YUV2GRAY_Y422, 0);
  GaussianBlur(M,img_blur, Size(5,5), BORDER_DEFAULT);

  threshold(img_blur, img_blur, 86, 255, THRESH_BINARY);

  Canny(img_blur,dst,40,255,3,false);

  // Mat element = getStructuringElement(MORPH_RECT,
  //                   Size(2*1 +1,2*1+1),
  //                   Point(-1,-1));
  // dilate(dst, dilation_dst, element);

  grayscale_opencv_to_yuv422(dst, img, width, height);

  return 0;
}
