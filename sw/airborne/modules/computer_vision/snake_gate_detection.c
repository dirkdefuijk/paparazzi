
/*
 * Copyright (C) 2018, Guido de Croon and Michael Ozo
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
 * @file modules/computer_vision/snake_gate_detection.c
 */

// Own header
#include "modules/computer_vision/snake_gate_detection.h"
#include <stdio.h>
#include <stdlib.h>
#include "modules/computer_vision/lib/vision/image.h"

// to debug the algorithm, uncomment the define:
// #define DEBUG_SNAKE_GATE

// the return values of the main detection function:
#define SUCCESS_DETECT 1
#define FAIL_DETECT 0

// whether to filter the image and show the best gate(s):
#define FILTER_IMAGE 1
#define DRAW_GATE 1

// Standard colors in UYVY:
uint8_t green_color[4] = {255, 128, 255, 128};
uint8_t blue_color[4] = {0, 128, 0, 128};
uint8_t white_color[4] = {255, 255, 255, 255};

// Filter Settings
uint8_t color_Y_min;
uint8_t color_Y_max;
uint8_t color_U_min;
uint8_t color_U_max;
uint8_t color_V_min;
uint8_t color_V_max;
// Other settings:
int min_pixel_size;

// Result
#define MAX_GATES 50
struct gate_img gates_c[MAX_GATES];
struct gate_img best_gate;
struct gate_img temp_check_gate;
struct image_t img_result;
int n_gates = 0;
float best_quality = 0;
float current_quality = 0;
float best_fitness = 100000;
int last_frame_detection = 0;
int repeat_gate = 0;
// previous best gate:
struct gate_img previous_best_gate = {0};
struct gate_img last_gate;


// Support functions:
int cmpfunc(const void *a, const void *b);
int cmp_i(const void *a, const void *b);
float segment_length(struct point_t Q1, struct point_t Q2);
int bound_value_int(int input, int min, int max);

int cmpfunc(const void *a, const void *b)
{
  return (*(const int *)a - * (const int *)b);
}
int *array;
//return indexes
int cmp_i(const void *a, const void *b)
{
  int ia = *(const int *)a;
  int ib = *(const int *)b;
  return array[ia] < array[ib] ? -1 : array[ia] > array[ib];
}

// TODO: NOT FOR A FIRST PULL REQUEST: Since coordinates matter here, we have to deal with the strange sensor mounting in the Parrot Bebop.
//       This leads to checks such as x < im->h... This is a quite fundamental problem, with not a clear solution. However, if a normally
//       mounted sensor is used, the functions here will fail on this exact point...
//
// AXIS system
// (0,0)   im->w
//       Y
// -|----->
//  |
//  |
//  |
//  |
//  \/
// X
// im->h


/**
 * Run snake gate detection on an image. It assumes that it gets images over time, and remembers previous detections.
 *
 * Snake gate takes samples from an image. If a sample corresponds to the target color,
 * it will then go up and down, and afterwards left and right, in order to find chains
 * of pixels of the right color. This forms the initial guess for a square approximation
 * to the gate. Then, the estimate is refined, by looking around the supposed corner locations.
 * This leads to a polygon gate.
 *
 * @param[out] success Whether a gate was detected
 * @param[in] img The input image. We will draw in it.
 * @param[in] n_samples The number of samples taken to find a gate - proportional to the computational effort and detection performance.
 * @param[in] min_px_size The minimum pixel size an initial, purely square detection should have
 * @param[in] min_gate_quality How much percentage of the initial square outline needs to have the target color.
 * @param[in] gate_thickness After snaking, how much the extreme coordinates have to be adjusted.
 * @param[in] color_ym The Y minimum value
 * @param[in] color_yM The Y maximum value
 * @param[in] color_um The U minimum value
 * @param[in] color_uM The U maximum value
 * @param[in] color_vm The V minimum value
 * @param[in] color_vM The V maximum value
 * @param[in] *best_gate This gate_img struct will be filled with the data of the best detected gate.
 */

int snake_gate_detection(struct image_t *img, int n_samples, int min_px_size, float min_gate_quality, float gate_thickness, int min_n_sides,
                         uint8_t color_Ym, uint8_t color_YM, uint8_t color_Um, uint8_t color_UM, uint8_t color_Vm, uint8_t color_VM, struct gate_img* best_gate)
{

  color_Y_min = color_Ym;
  color_Y_max = color_YM;
  color_U_min  = color_Um;
  color_U_max  = color_UM;
  color_V_min  = color_Vm;
  color_V_max  = color_VM;
  min_pixel_size = min_px_size;

  int x, y;
  float quality;
  best_quality = 0;
  (*best_gate).gate_q = 0;
  n_gates = 0;

  // variables for snake gate detection:
  int y_low = 0;
  int y_high = 0;
  int x_low1 = 0;
  int x_high1 = 0;
  int x_low2 = 0;
  int x_high2 = 0;
  int sz = 0;
  int szx1 = 0;
  int szx2 = 0;

  for (int i = 0; i < n_samples; i++) {
    // get a random coordinate:
    x = rand() % img->h;
    y = rand() % img->w;

    // check if it has the right color
    if (check_color_sgd(img, x, y)) {

      // fill histogram (TODO: for a next pull request, in which we add the close-by histogram-detection)
      // histogram[x]++;

      // snake up and down:
      snake_up_and_down(img, x, y, &y_low, &y_high);

      // This assumes the gate to be square:
      sz = y_high - y_low;
      y_low = y_low + (sz * gate_thickness);
      y_high = y_high - (sz * gate_thickness);
      y = (y_high + y_low) / 2;

      // if the found part of the gate is large enough
      if (sz > min_pixel_size) {

        // snake left and right, both for the top and bottom part of the gate:
        snake_left_and_right(img, x, y_low, &x_low1, &x_high1);
        snake_left_and_right(img, x, y_high, &x_low2, &x_high2);
        x_low1 = x_low1 + (sz * gate_thickness);
        x_high1 = x_high1 - (sz * gate_thickness);
        x_low2 = x_low2 + (sz * gate_thickness);
        x_high2 = x_high2 - (sz * gate_thickness);

        // sizes of the left-right stretches: in y pixel coordinates
        szx1 = (x_high1 - x_low1);
        szx2 = (x_high2 - x_low2);

        // set the size according to the biggest detection:
        if (szx1 > szx2) {
          // determine the center x based on the bottom part:
          x = (x_high1 + x_low1) / 2;
          // set the size to the largest line found:
          sz = (sz > szx1) ? sz : szx1;
        } else {
          // determine the center x based on the top part:
          x = (x_high2 + x_low2) / 2;
          // set the size to the largest line found:
          sz = (sz > szx2) ? sz : szx2;
        }

        if (sz > min_pixel_size) {
          // create the gate:
          gates_c[n_gates].x = x;
          gates_c[n_gates].y = y;
          // store the half gate size:
          gates_c[n_gates].sz = sz / 2;

          // check the gate quality:
          check_gate_initial(img, gates_c[n_gates], &quality, &gates_c[n_gates].n_sides);
          gates_c[n_gates].gate_q = quality;

          // only increment the number of gates if the quality is better
          // else it will be overwritten by the next one
          if (quality > best_quality) {
            best_quality = quality;
            n_gates++;
          }
        }

        if (n_gates >= MAX_GATES) {
          break;
        }
      }
    }
  }

/*
#ifdef DEBUG_SNAKE_GATE
  // draw all candidates:
  printf("n_gates:%d\n", n_gates);
  for (int i = 0; i < n_gates; i++) {
    draw_gate_color_square(img, gates_c[i], white_color);
  }
#endif
*/
  //init best gate
  (*best_gate).gate_q = 0;
  (*best_gate).n_sides = 0;
  repeat_gate = 0;

  // do an additional fit to improve the gate detection:
  // if the current best gate is good enough
  // OR if we had a detection last frame, so the gate should still
  if ((best_quality > min_gate_quality && n_gates > 0) || last_frame_detection) {

    int max_candidate_gates = 10;
    best_fitness = 100;

    int initial_gate = n_gates - max_candidate_gates;
    initial_gate = (initial_gate < 0) ? 0 : initial_gate;

    for (int gate_nr = initial_gate; gate_nr < n_gates; gate_nr += 1) {

      // get gate information:
      set_gate_points(&gates_c[gate_nr]);
      gate_refine_corners(img, gates_c[gate_nr].x_corners, gates_c[gate_nr].y_corners, gates_c[gate_nr].sz*2);

      // TODO: I think we can remove the temporary gate here, and just use gates_c[gate_nr]
      // store the temporary information in the gate:
      temp_check_gate.x = gates_c[gate_nr].x;
      temp_check_gate.y = gates_c[gate_nr].y;
      temp_check_gate.sz = gates_c[gate_nr].sz;
      memcpy(&(temp_check_gate.x_corners[0]), gates_c[gate_nr].x_corners, sizeof(int) * 4);
      memcpy(&(temp_check_gate.y_corners[0]), gates_c[gate_nr].y_corners, sizeof(int) * 4);

      // also get the color fitness
      check_gate_outline(img, temp_check_gate, &temp_check_gate.gate_q, &temp_check_gate.n_sides);

      // If the gate is good enough:
      if (temp_check_gate.n_sides >= min_n_sides && temp_check_gate.gate_q > (*best_gate).gate_q) {
        // store the information in the gate:
        (*best_gate).x = temp_check_gate.x;
        (*best_gate).y = temp_check_gate.y;
        (*best_gate).sz = temp_check_gate.sz;
        (*best_gate).sz_left = temp_check_gate.sz_left;
        (*best_gate).sz_right = temp_check_gate.sz_right;
        (*best_gate).gate_q = temp_check_gate.gate_q;
        (*best_gate).n_sides = temp_check_gate.n_sides;
        memcpy(&((*best_gate).x_corners[0]), &(temp_check_gate.x_corners[0]), sizeof(int) * 4);
        memcpy(&((*best_gate).y_corners[0]), &(temp_check_gate.y_corners[0]), sizeof(int) * 4);
      }
    }

    // if the best gate is not good enough, but we did have a detection in the previous image:
    if (((*best_gate).gate_q == 0 && (*best_gate).n_sides == 0) && last_frame_detection == 1) {

      // TODO: is it really important to do this sorting here to get the maximum size? Is the sz property not accurate enough?
      int x_values[4];
      int y_values[4];
      memcpy(x_values, &(last_gate.x_corners[0]), sizeof(int) * 4);
      memcpy(y_values, &(last_gate.y_corners[0]), sizeof(int) * 4);
      //sort small to large
      qsort(x_values, 4, sizeof(int), cmpfunc);
      qsort(y_values, 4, sizeof(int), cmpfunc);
      //check x size, maybe use y also later?
      int radius_p   = x_values[3] - x_values[0];
      gate_refine_corners(img, last_gate.x_corners, last_gate.y_corners, 2*radius_p);

      // also get the color fitness
      check_gate_outline(img, last_gate, &last_gate.gate_q, &last_gate.n_sides);

      // if the refined detection is good enough:
      if (last_gate.n_sides >= min_n_sides && last_gate.gate_q > (*best_gate).gate_q) {
        repeat_gate = 1;
        (*best_gate).gate_q = last_gate.gate_q;
        (*best_gate).n_sides = last_gate.n_sides;
        memcpy(&((*best_gate).x_corners[0]), &(last_gate.x_corners[0]), sizeof(int) * 4);
        memcpy(&((*best_gate).y_corners[0]), &(last_gate.y_corners[0]), sizeof(int) * 4);
      }
    }

    #ifdef DEBUG_SNAKE_GATE
      // draw the best gate:
      draw_gate(img, (*best_gate));
    #endif

  }

  // prepare for the next time:
  previous_best_gate.x = (*best_gate).x;
  previous_best_gate.y = (*best_gate).y;
  previous_best_gate.sz = (*best_gate).sz;
  previous_best_gate.sz_left = (*best_gate).sz_left;
  previous_best_gate.sz_right = (*best_gate).sz_right;
  previous_best_gate.gate_q = (*best_gate).gate_q;
  previous_best_gate.n_sides = (*best_gate).n_sides;
  memcpy(&(previous_best_gate.x_corners[0]), &((*best_gate).x_corners[0]), sizeof(int) * 4);
  memcpy(&(previous_best_gate.y_corners[0]), &((*best_gate).y_corners[0]), sizeof(int) * 4);

  //color filtered version of image for overlay and debugging
  if (FILTER_IMAGE) { //filter) {
    image_yuv422_colorfilt(img, img, color_Y_min, color_Y_max, color_U_min, color_U_max, color_V_min, color_V_max );
  }

  if ((*best_gate).gate_q > (min_gate_quality * 2) && (*best_gate).n_sides > min_n_sides) {

    // successful detection
    last_frame_detection = 1;

    //draw_gate_color(img, best_gate, blue_color);
    if (DRAW_GATE) {
      int size_crosshair = 10;
      if (repeat_gate == 0) {
        draw_gate_color_polygon(img, (*best_gate), blue_color);
      } else if (repeat_gate == 1) {
        draw_gate_color_polygon(img, (*best_gate), green_color);
        for (int i = 0; i < 3; i++) {
          struct point_t loc = { .x = last_gate.x_corners[i], .y = last_gate.y_corners[i] };
          image_draw_crosshair(img, &loc, blue_color, size_crosshair);
        }
      }
    }
    //save for next iteration
    memcpy(&(last_gate.x_corners[0]), &((*best_gate).x_corners[0]), sizeof(int) * 4);
    memcpy(&(last_gate.y_corners[0]), &((*best_gate).y_corners[0]), sizeof(int) * 4);
    //previous best snake gate
    last_gate.x = (*best_gate).x;
    last_gate.y = (*best_gate).y;
    last_gate.sz = (*best_gate).sz;
    current_quality = best_quality;

    //SIGNAL NEW DETECTION AVAILABLE
    return SUCCESS_DETECT;

  } else {
    //no detection
    last_frame_detection = 0;
    current_quality = 0;
    return FAIL_DETECT;
  }
}


/**
 * Draw the gate on an image.
 *
 * @param[in] img The output image.
 * @param[in] gate The gate to be drawn.
 */
void draw_gate(struct image_t *im, struct gate_img gate)
{
  draw_gate_color_polygon(im, gate, white_color);
}


/**
 * Draw the gate on an image, using the corner points, possibly resulting in a polygon.
 *
 * @param[in] img The output image.
 * @param[in] gate The gate to be drawn.
 * @param[in] color The color of the lines, in UYVY format.
 */
void draw_gate_color_polygon(struct image_t *im, struct gate_img gate, uint8_t *color)
{
  // Please note that here we use functions in image.h, so we have to inverse the coordinates:
  // draw four lines and a crosshair on the image:
  struct point_t from, to;

  // a cross at the center
  from.x = gate.y;
  from.y = gate.x;
  image_draw_crosshair(im, &from, color, 10);

  // the four lines:
  from.x = gate.y_corners[0];
  from.y = gate.x_corners[0];
  to.x = gate.y_corners[1];
  to.y = gate.x_corners[1];
  image_draw_line_color(im, &from, &to, color);

  from.x = gate.y_corners[1];
  from.y = gate.x_corners[1];
  to.x = gate.y_corners[2];
  to.y = gate.x_corners[2];
  image_draw_line_color(im, &from, &to, color);

  from.x = gate.y_corners[2];
  from.y = gate.x_corners[2];
  to.x = gate.y_corners[3];
  to.y = gate.x_corners[3];
  image_draw_line_color(im, &from, &to, color);

  from.x = gate.y_corners[3];
  from.y = gate.x_corners[3];
  to.x = gate.y_corners[0];
  to.y = gate.x_corners[0];
  image_draw_line_color(im, &from, &to, color);

}



/**
 * Draw the gate on an image, using only the center coordinate and sizes - resulting in a square gate.
 *
 * @param[in] img The output image.
 * @param[in] gate The gate to be drawn.
 * @param[in] color The color of the lines, in UYVY format.
 */
void draw_gate_color_square(struct image_t *im, struct gate_img gate, uint8_t *color)
{
  // Please note that here we use functions in image.h, so we have to inverse the coordinates:
  // draw four lines and a crosshair on the image:
  struct point_t from, to;

  from.x = gate.y;
  from.y = gate.x;
  image_draw_crosshair(im, &from, color, 10);



  if (gate.sz_left == gate.sz_right) {
    // square
    from.x = gate.y - gate.sz;
    from.y = gate.x - gate.sz;
    to.x = gate.y + gate.sz;
    to.y = gate.x - gate.sz;
    image_draw_line_color(im, &from, &to, color);
    from.x = gate.y + gate.sz;
    from.y = gate.x - gate.sz;
    to.x = gate.y + gate.sz;
    to.y = gate.x + gate.sz;
    image_draw_line_color(im, &from, &to, color);
    from.x = gate.y + gate.sz;
    from.y = gate.x + gate.sz;
    to.x = gate.y - gate.sz;
    to.y = gate.x + gate.sz;
    image_draw_line_color(im, &from, &to, color);
    from.x = gate.y - gate.sz;
    from.y = gate.x + gate.sz;
    to.x = gate.y - gate.sz;
    to.y = gate.x - gate.sz;
    image_draw_line_color(im, &from, &to, color);
  } else {
    from.x = gate.y - gate.sz_left;
    from.y = gate.x - gate.sz;
    to.x = gate.y + gate.sz_left;
    to.y = gate.x - gate.sz;
    image_draw_line_color(im, &from, &to, color);
    from.x = gate.y + gate.sz_left;
    from.y = gate.x - gate.sz;
    to.x = gate.y + gate.sz_right;
    to.y = gate.x + gate.sz;
    image_draw_line_color(im, &from, &to, color);
    from.x = gate.y + gate.sz_right;
    from.y = gate.x + gate.sz;
    to.x = gate.y - gate.sz_right;
    to.y = gate.x + gate.sz;
    image_draw_line_color(im, &from, &to, color);
    from.x = gate.y - gate.sz_right;
    from.y = gate.x + gate.sz;
    to.x = gate.y - gate.sz_left;
    to.y = gate.x - gate.sz;
    image_draw_line_color(im, &from, &to, color);
  }
}

/**
 * Check only the outline of the gate. The gate must have corners already assigned in the struct.
 *
 * @param[in] im The input image.
 * @param[in] gate The gate to be checked.
 * @param[in] quality The ratio of rightly colored pixels, 1.0 is best, 0.0 is worst.
 * @param[in] sides How many of the sides are sufficiently colored?
 */
void check_gate_outline(struct image_t *im, struct gate_img gate, float *quality, int *n_sides)
{
  int n_points, n_colored_points;
  n_points = 0;
  n_colored_points = 0;
  int np, nc;

  // how much of the side should be visible to count as a detected side?
  // TODO: make this a setting. Note: the number is different from the check_gate_initial function.
  float min_ratio_side = 0.4;
  (*n_sides) = 0;

  float min_segment_length = min_pixel_size;

  // check the four lines of which the gate consists:
  struct point_t from, to;

  from.x = gate.x_corners[0];
  from.y = gate.y_corners[0];
  to.x = gate.x_corners[1];
  to.y = gate.y_corners[1];
  check_line(im, from, to, &np, &nc);
  if ((float) nc / (float) np >= min_ratio_side && segment_length(from, to) > min_segment_length) {
    (*n_sides)++;
  }
  n_points += np;
  n_colored_points += nc;

  from.x = gate.x_corners[1];
  from.y = gate.y_corners[1];
  to.x = gate.x_corners[2];
  to.y = gate.y_corners[2];
  check_line(im, from, to, &np, &nc);
  if ((float) nc / (float) np >= min_ratio_side && segment_length(from, to) > min_segment_length) {
    (*n_sides)++;
  }
  n_points += np;
  n_colored_points += nc;

  from.x = gate.x_corners[2];
  from.y = gate.y_corners[2];
  to.x = gate.x_corners[3];
  to.y = gate.y_corners[3];
  check_line(im, from, to, &np, &nc);
  if ((float) nc / (float) np >= min_ratio_side && segment_length(from, to) > min_segment_length) {
    (*n_sides)++;
  }
  n_points += np;
  n_colored_points += nc;

  from.x = gate.x_corners[3];
  from.y = gate.y_corners[3];
  to.x = gate.x_corners[0];
  to.y = gate.y_corners[0];
  check_line(im, from, to, &np, &nc);
  if ((float) nc / (float) np >= min_ratio_side && segment_length(from, to) > min_segment_length) {
    (*n_sides)++;
  }

  n_points += np;
  n_colored_points += nc;

  // the quality is the ratio of colored points / number of points:
  if (n_points == 0) {
    (*quality) = 0;
  } else {
    (*quality) = ((float) n_colored_points) / ((float) n_points);
  }
}


/**
 * Check the outline and the center of the gate. The outline should be of the right color.
 * The inner part should be a different color, or else we may be looking at a distractor object.
 * The gate does not yet have to have corners assigned in the struct.
 *
 * @param[in] im The input image.
 * @param[in] gate The gate to be checked.
 * @param[in] quality The ratio of rightly colored pixels, 1.0 is best, 0.0 is worst.
 *            If too many pixels inside the gate are of the right color, the quality is also 0.
 *  @param[in] sides How many of the sides are sufficiently colored?
 */
extern void check_gate_initial(struct image_t *im, struct gate_img gate, float *quality, int *n_sides)
{
  int n_points, n_colored_points;
  n_points = 0;
  n_colored_points = 0;
  int np, nc;

  // how much of the side should be visible to count as a detected side?
  float min_ratio_side = 0.30;
  (*n_sides) = 0;

  // check the four lines of which the gate consists:
  struct point_t from, to;

  from.x = gate.x - gate.sz;
  from.y = gate.y - gate.sz_left;
  to.x = gate.x - gate.sz;
  to.y = gate.y + gate.sz_left;
  check_line(im, from, to, &np, &nc);
  if ((float) nc / (float) np >= min_ratio_side) {
    (*n_sides)++;
  }
  n_points += np;
  n_colored_points += nc;

  from.x = gate.x - gate.sz;
  from.y = gate.y + gate.sz_left;
  to.x = gate.x + gate.sz;
  to.y = gate.y + gate.sz_right;
  check_line(im, from, to, &np, &nc);
  if ((float) nc / (float) np >= min_ratio_side) {
    (*n_sides)++;
  }
  n_points += np;
  n_colored_points += nc;

  from.x = gate.x + gate.sz;
  from.y = gate.y + gate.sz_right;
  to.x = gate.x + gate.sz;
  to.y = gate.y - gate.sz_right;
  check_line(im, from, to, &np, &nc);
  if ((float) nc / (float) np >= min_ratio_side) {
    (*n_sides)++;
  }
  n_points += np;
  n_colored_points += nc;

  from.x = gate.x + gate.sz;
  from.y = gate.y - gate.sz_right;
  to.x = gate.x - gate.sz;
  to.y = gate.y - gate.sz_left;
  check_line(im, from, to, &np, &nc);
  if ((float) nc / (float) np >= min_ratio_side) {
    (*n_sides)++;
  }

  n_points += np;
  n_colored_points += nc;


  // the quality is the ratio of colored points / number of points:
  if (n_points == 0) {
    (*quality) = 0;
  } else {
    (*quality) = ((float) n_colored_points) / ((float) n_points);
  }

  // check that the inside of the gate is not of the target color as well:
  int n_samples_in = 20;
  float num_color_center = 0;
  float center_discard_threshold = 0.25;
  for (int i = 0; i < n_samples_in; i++) {
    // get a random coordinate:
    int x_in = gate.x + (rand() % gate.sz) - (0.5 * gate.sz);
    int y_in = gate.y + (rand() % gate.sz) - (0.5 * gate.sz);

    // check if it has the right color
    if (check_color_sgd(im, x_in, y_in)) {
      num_color_center ++;
    }
  }
  //how much center pixels colored?
  float center_factor = num_color_center / (float)n_samples_in;
  if (center_factor > center_discard_threshold) { (*quality) = 0; }

}

/**
 * Determine the segment length between two 2D-points.
 *
 * @param[in] Q1 Point 1.
 * @param[in] Q2 Point 2.
 */
float segment_length(struct point_t Q1, struct point_t Q2)
{

  float r = sqrt((Q1.x - Q2.x) * (Q1.x - Q2.x) + (Q1.y - Q2.y) * (Q1.y - Q2.y));
  return r;
}

/**
 * Checks whether points on a line between two 2D-points are of a given color.
 *
 * @param[in] im The input image.
 * @param[in] Q1 Point 1.
 * @param[in] Q2 Point 2.
 * @param[in] n_points The number of sampled points.
 * @param[in] n_colored_points The number of sampled points that were of the right color.
 */
void check_line(struct image_t *im, struct point_t Q1, struct point_t Q2, int *n_points, int *n_colored_points)
{

  (*n_points) = 0;
  (*n_colored_points) = 0;

  // t_step determines how many samples are taken (1.0 / t_step)
  float t_step = 0.05;
  int x, y;
  float t;
  // go from Q1 to Q2 in 1/t_step steps:
  for (t = 0.0f; t < 1.0f; t += t_step) {
    // determine integer coordinate on the line:
    x = (int)(t * Q1.x + (1.0f - t) * Q2.x);
    y = (int)(t * Q1.y + (1.0f - t) * Q2.y);

    // Is the point in the image?
    if (x >= 0 && x < im->h && y >= 0 && y < im->w) {
      // augment number of checked points:
      (*n_points)++;

      if (check_color_sgd(im, x, y)) {
        // the point is of the right color:
        (*n_colored_points)++;
      }
    }
  }
}

/**
 * The actual snaking. An "agent" starts at (x,y) and goes as far as possible up and down (y-direction),
 * keeping a chain of "connected" target color pixels. The agent can go slightly to the right and left.
 *
 * @param[in] im The input image.
 * @param[in] x The initial x-coordinate
 * @param[in] y The initial y-coordinate
 * @param[in] y_low The current lowest y-estimate
 * @param[in] y_high The current highest y-estimate
 */
void snake_up_and_down(struct image_t *im, int x, int y, int *y_low, int *y_high)
{
  int done = 0;
  int x_initial = x;
  (*y_low) = y;

  // snake towards negative y
  while ((*y_low) > 0 && !done) {
    if (check_color_sgd(im, x, (*y_low) - 1)) {
      (*y_low)--;

    } else if (x + 1 < im->h && check_color_sgd(im, x + 1, (*y_low) - 1)) {
      x++;
      (*y_low)--;
    } else if (x - 1 >= 0 && check_color_sgd(im, x - 1, (*y_low) - 1)) {
      x--;
      (*y_low)--;
    } else {
      done = 1;
    }
  }

  // snake towards positive y
  x = x_initial;
  (*y_high) = y;
  done = 0;
  while ((*y_high) < im->w - 1 && !done) {

    if (check_color_sgd(im, x, (*y_high) + 1)) {
      (*y_high)++;
    } else if (x < im->h - 1 && check_color_sgd(im, x + 1, (*y_high) + 1)) {
      x++;
      (*y_high)++;
    } else if (x > 0 && check_color_sgd(im, x - 1, (*y_high) + 1)) {
      x--;
      (*y_high)++;
    } else {
      done = 1;
    }
  }
}


/**
 * The actual snaking. An "agent" starts at (x,y) and goes as far as possible left and right (x-direction),
 * keeping a chain of "connected" target color pixels. The agent can go slightly to the top and bottom.
 *
 * @param[in] im The input image.
 * @param[in] x The initial x-coordinate
 * @param[in] y The initial y-coordinate
 * @param[in] x_low The current lowest x-estimate
 * @param[in] x_high The current highest x-estimate
 */
void snake_left_and_right(struct image_t *im, int x, int y, int *x_low, int *x_high)
{
  int done = 0;
  int y_initial = y;
  (*x_low) = x;

  // snake towards negative x (left)
  while ((*x_low) > 0 && !done) {
    if (check_color_sgd(im, (*x_low) - 1, y)) {
      (*x_low)--;
      // } else if (y < im->h - 1 && check_color_sgd(im, (*x_low) - 1, y + 1)) {
    } else if (y < im->w - 1 && check_color_sgd(im, (*x_low) - 1, y + 1)) {
      y++;
      (*x_low)--;
    } else if (y > 0 && check_color_sgd(im, (*x_low) - 1, y - 1)) {
      y--;
      (*x_low)--;
    } else {
      done = 1;
    }
  }

  y = y_initial;
  (*x_high) = x;
  done = 0;
  // snake towards positive x (right)
  while ((*x_high) < im->h - 1 && !done) {

    if (check_color_sgd(im, (*x_high) + 1, y)) {
      (*x_high)++;
      // } else if (y < im->h - 1 && check_color_sgd(im, (*x_high) + 1, y++)) {
    } else if (y < im->w - 1 && check_color_sgd(im, (*x_high) + 1, y++)) {
      y++;
      (*x_high)++;
    } else if (y > 0 && check_color_sgd(im, (*x_high) + 1, y - 1)) {
      y--;
      (*x_high)++;
    } else {
      done = 1;
    }
  }
}

/**
 * Determine and set the corner locations in gate.x_corners, g.y_corners, based
 * on the center of the gate and its size, assuming a square window.
 *
 * @param[in] gate The gate struct with the relevant information.
 */
void set_gate_points(struct gate_img *gate)
{

  gate->x_corners[0] = gate->x - gate->sz;
  gate->y_corners[0] = gate->y + gate->sz;
  gate->x_corners[1] = gate->x + gate->sz;
  gate->y_corners[1] = gate->y + gate->sz;
  gate->x_corners[2] = gate->x + gate->sz;
  gate->y_corners[2] = gate->y - gate->sz;
  gate->x_corners[3] = gate->x - gate->sz;
  gate->y_corners[3] = gate->y - gate->sz;

}

/**
 * Refine the four corners of the gate, based on the color around the supposed corner locations.
 *
 * @param[in] color_image The color image.
 * @param[in] x_points The x-coordinates of the gate. These will be updated.
 * @param[in] y_points The y-coordinates of the gate. These will be updated.
 * @param[in] size Full size of the gate.
 */
void gate_refine_corners(struct image_t *color_image, int *x_points, int *y_points, int size)
{

  // TODO: make parameter?
  float corner_area = 0.4f;
  refine_single_corner(color_image, x_points, y_points, size, corner_area);
  refine_single_corner(color_image, x_points + 1, y_points + 1, size, corner_area);
  refine_single_corner(color_image, x_points + 2, y_points + 2, size, corner_area);
  refine_single_corner(color_image, x_points + 3, y_points + 3, size, corner_area);
}

// TODO: there is a bound function in ctrlstick.c, but I did not want to include such files. Perhaps there is a better place than here.
/**
 * Bound function.
 *
 * @param[in] input The value to be bounded.
 * @param[in] min The minimum allowed value.
 * @param[in] max The maximum allowed value.
 */
int bound_value_int(int input, int min, int max){
  if(input < min){
    return min;
  }
  else if(input > max){
    return max;
  }
  else{
    return input;
  }
}

/**
 * Refine a single corner, based on the color around the coordinate.
 *
 * @param[in] im The color image.
 * @param[in] corner_x The corner's initial x-coordinate
 * @param[in] corner_y The corner's initial y-coordinate
 * @param[in] size Full size of the gate.
 * @param[in] size_factor The ratio of the size in which we will look for a better corner location.
 */
void refine_single_corner(struct image_t *im, int* corner_x, int* corner_y, int size, float size_factor)
{

  float x_corner_f = (float)(*corner_x);
  float y_corner_f = (float)(*corner_y);
  float size_f     = (float)size;
  size_factor = 0.4f;

  int x_l = (int)(x_corner_f - size_f * size_factor);
  x_l = bound_value_int(x_l, 0, im->h);
  int x_r = (int)(x_corner_f + size_f * size_factor);
  x_r = bound_value_int(x_r, 0, im->h);
  int y_h = (int)(y_corner_f + size_f * size_factor);
  y_h = bound_value_int(y_h, 0, im->w);
  int y_l = (int)(y_corner_f - size_f * size_factor);
  y_l = bound_value_int(y_l, 0, im->w);

  int x_size = x_r - x_l + 1;
  int y_size = y_h - y_l + 1;

  int x_hist[x_size];
  int y_hist[y_size];
  memset(x_hist, 0, sizeof(int)*x_size);
  memset(y_hist, 0, sizeof(int)*y_size);


  int best_x = 0;
  int best_x_loc = x_l;
  int x_best_start = x_l;
  int best_y = 0;
  int best_y_loc = y_l;
  int y_best_start = y_l;


  for (int y_pix = y_l; y_pix < y_h; y_pix++) {
    for (int x_pix = x_l; x_pix < x_r; x_pix++) {
      if (check_color_sgd(im, x_pix, y_pix) > 0) {

        int cur_x = x_hist[x_pix - x_l];
        int cur_y = y_hist[y_pix - y_l];
        x_hist[x_pix - x_l] = cur_x + 1;
        y_hist[y_pix - y_l] = cur_y + 1;

        if (x_hist[x_pix - x_l] > best_x) {
          best_x = x_hist[x_pix - x_l];
          best_x_loc = x_pix;
          x_best_start = x_pix;
        } else if (cur_x == best_x) {
          best_x_loc = (x_pix + x_best_start) / 2;
        }
        if (y_hist[y_pix - y_l] > best_y) {
          best_y = y_hist[y_pix - y_l];
          best_y_loc = y_pix;
          y_best_start = y_pix;
        } else if (cur_y == best_y) {
          best_y_loc = (y_pix + y_best_start) / 2;
        }

      }
    }
  }

  // Update the corner location:
  *corner_x = best_x_loc;
  *corner_y = best_y_loc;
}


int check_color_sgd(struct image_t *im, int x, int y) {

  // Call the function in image.c with the color thresholds:
  // Please note that we have to switch x and y around here, due to the strange sensor mounting in the Bebop:
  int success = check_color(im, y, x, color_Y_min, color_Y_max, color_U_min, color_U_max, color_V_min, color_V_max);

  /*
  #ifdef DEBUG_SNAKE_GATE
    if(success) {
      set_color(im, y, x, 0, 0, 0);
    }
  #endif
  */

  return success;
}
