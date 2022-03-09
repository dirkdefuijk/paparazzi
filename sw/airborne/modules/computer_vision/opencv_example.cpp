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
int partition7(Mat image){
  static int sections[7];
  height = (int) sizeof(image[0]);
  width  = (int) sizeof(image[1]);
  // Cut the image in half
  width_cutoff = width / 3
  left1 = img[:, :width_cutoff]
  mid1 = img[:, width_cutoff:(2*width_cutoff)]
  right1 = img[:, (2*width_cutoff):]
  // finish vertical devide image
  /* ##########################################
  # At first Horizontal devide left1 image #
  ########################################## */
  //rotate image LEFT1 to 90 CLOCKWISE
  img = cv2.rotate(left1, cv2.ROTATE_90_CLOCKWISE)
  // start vertical devide image

  width = img.shape[1]
  // Cut the image in half
  width_cutoff = width // 2
  l1 = img[:, :width_cutoff]
  l2 = img[:, width_cutoff:]
  // finish vertical devide image
  // rotate image to 90 COUNTERCLOCKWISE
  l1 = cv2.rotate(l1, cv2.ROTATE_90_COUNTERCLOCKWISE)
  /*##########################################
  # At first Horizontal devide right1 image#
  ##########################################*/
  // rotate image RIGHT1 to 90 CLOCKWISE
  img = cv2.rotate(right1, cv2.ROTATE_90_CLOCKWISE)
  // start vertical devide image
  width = img.shape[1]
  // Cut the image in half
  width_cutoff = width // 2
  r1 = img[:, :width_cutoff]
  r2 = img[:, width_cutoff:]
  // finish vertical devide image
  //rotate image to 90 COUNTERCLOCKWISE
  r1 = cv2.rotate(r1, cv2.ROTATE_90_COUNTERCLOCKWISE)
  img = cv2.rotate(right1, cv2.ROTATE_90_CLOCKWISE)
  width = img.shape[1]
  width_cutoff = width // 3
  m1 = img[:, :width_cutoff]
  m2 = img[:, width_cutoff:(2*width_cutoff)]
  m3 = img[:, (2*width_cutoff):]
  sections = [l1,l2,m1,m2,m3,r1,r2]
  return sections
}

int opencv_example(char *img, int width, int height)
{


/** @file "modules/wedgebug/wedgebug.h" 
 * function : // Function 3 - Creates empty 8bit kernel
 * void kernel_create(struct kernel_C1 *kernel, uint16_t width, uint16_t height, enum image_type type) 
 *   // Creating empty kernel:
 * kernel_create(&median_kernel, kernel_median_dims.w, kernel_median_dims.h, IMAGE_GRAYSCALE);
 * kernel_create(&median_kernel16bit, kernel_median_dims.w, kernel_median_dims.h, IMAGE_INT16);
 * // Structures
  // Kernel - processes single channel images 
struct kernel_C1 {
  enum image_type type;   // Type of image on which kernel is laid onto
  uint16_t w;           //s/< Kernel width
  uint16_t h;           ///< Kernel height
  uint32_t buf_size;    ///< Size of values of weight buffer and values buffer
  void *buf_weights;    ///< Kernel weight buffer
  void *buf_values;     ///< Kernel value buffer. These are the values underneath the kernel
};
**/

  // // Canny edges, only works with grayscale image
  // int edgeThresh = 35;
  // Canny(image, image, edgeThresh, edgeThresh * 3);
  // // Convert back to YUV422, and put it in place of the original image
  // grayscale_opencv_to_yuv422(image, img, width, height);

  // // Color image example
  // // Convert the image to an OpenCV Mat
  // cvtColor(M, image, CV_YUV2BGR_Y422);
  // // Blur it, because we can
  // blur(image, image, Size(5, 5));
  // // Convert back to YUV422 and put it in place of the original image
  // colorbgr_opencv_to_yuv422(image, img, width, height);


  return 0;
}
