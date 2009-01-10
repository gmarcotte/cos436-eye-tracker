/*
 *
 * cvEyeTracker is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * cvEyeTracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cvEyeTracker; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * cvEyeTracker - Version 1.2.5
 * Part of the openEyes ToolKit -- http://hcvl.hci.iastate.edu/openEyes
 * Release Date:
 * Authors : Dongheng Li <dhli@iastate.edu>
 *           Derrick Parkhurst <derrick.parkhurst@hcvl.hci.iastate.edu>
 *           Jason Babcock <babcock@nyu.edu>
 *           David Winfield <dwinfiel@iastate.edu>
 * Copyright (c) 2004-2006
 * All Rights Reserved.
 *
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "remove_corneal_reflection.h"
#include "RansacEllipseFinder.h"
#include "svd.h"
#include "cvEyeTracker.h"

#include "cv.h"
#include "highgui.h"

#define UINT8 unsigned char

#ifndef PI
#define PI 3.141592653589
#endif

#define FIX_UINT8(x) ( (x)<0 ? 0 : ((x)>255 ? 255:(x)) )

//Init colors
CvScalar White = CV_RGB(255,255,255);
CvScalar Red = CV_RGB(255,0,0);
CvScalar Green = CV_RGB(0,255,0);
CvScalar Blue = CV_RGB(0,0,255);
CvScalar Yellow = CV_RGB(255,255,0);
int frame_number=0;


cvEyeTracker::cvEyeTracker(int width, int height)
{
	// Parameters that clients can update on the fly
	this->min_pupil_contour_points = 500;   
	this->max_pupil_contour_points = 10000;    
	this->pupil_size_tolerance = 1000;	//range of allowed pupil diameters
	this->max_contour_count = 20;

	//parameters for the algorithm
	this->edge_threshold = 20;					//threshold of pupil edge points detection
	this->rays = 18;										//number of rays to use to detect feature points
	this->min_feature_candidates = 10;	//minimum number of pupil feature candidates
	this->cr_window_size = 301;					//corneal refelction search window size

	this->pupil = cvPoint(0, 0);									//coordinates of pupil in tracker coordinate system
	this->corneal_reflection = cvPoint(0, 0);	//coordinates of corneal reflection in tracker coordinate system
	this->diff_vector = cvPoint(0, 0);					//vector between the corneal reflection and pupil
	this->corneal_reflection_r = 0;					//the radius of corneal reflection

	this->beta = 0.2;									//hysteresis factor for noise reduction

	this->width = width;
	this->height = height;
	this->eye_image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	this->threshold_image = cvCloneImage(this->eye_image);
	this->ellipse_image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

	this->intensity_factor_hori = (double*)malloc(height*sizeof(double)); 
	this->avg_intensity_hori = (double*)malloc(height*sizeof(double));
}


cvEyeTracker::~cvEyeTracker()
{
	cvReleaseImage(&(this->eye_image));
	cvReleaseImage(&(this->threshold_image));
	cvReleaseImage(&(this->ellipse_image));
	free(this->intensity_factor_hori);
	free(this->avg_intensity_hori);
}

// Accessors
CvPoint cvEyeTracker::getPupilCoordinates() {return this->pupil;}
CvPoint cvEyeTracker::getCornealReflectionCoordinates() {return this->corneal_reflection;}
CvPoint cvEyeTracker::getDifferenceVector() {return this->diff_vector;}
int cvEyeTracker::getCornealReflectionRadius() {return this->corneal_reflection_r;}


void cvEyeTracker::getEyeImage(IplImage* im)
{	
	cvConvertImage(this->eye_image, im);
}


void cvEyeTracker::getEllipseImage(IplImage* im)
{
	cvConvertImage(this->ellipse_image, im);
}


void cvEyeTracker::getThresholdImage(IplImage* im)
{
	cvConvertImage(this->threshold_image, im);
}


void cvEyeTracker::process(IplImage* original_eye)
{
  unsigned int i;
	int j;
  int *inliers_index;
  CvSize ellipse_axis;
  static int lost_frame_num = 0;
  cvZero(this->ellipse_image);

	cvConvertImage(original_eye, this->eye_image);

  cvSmooth(this->eye_image, this->eye_image, CV_GAUSSIAN, 5, 5);
  this->Reduce_Line_Noise(this->eye_image);  
  
  //corneal reflection
  remove_corneal_reflection(this->eye_image, this->threshold_image, 
														(int)this->ref.start_point.x, (int)this->ref.start_point.y, 
														this->cr_window_size, (int)this->eye_image->height/10, 
														this->corneal_reflection.x, this->corneal_reflection.y, this->corneal_reflection_r);  
  printf("corneal reflection: (%d, %d)\n", this->corneal_reflection.x, this->corneal_reflection.y);
  this->Draw_Cross(this->ellipse_image, 
									this->corneal_reflection.x, this->corneal_reflection.y, 
									15, 15, Yellow);  

  //starburst pupil contour detection
  this->ref.starburst_pupil_contour_detection((UINT8*)this->eye_image->imageData, 
																							this->eye_image->width, this->eye_image->height,
																							this->edge_threshold, this->rays, this->min_feature_candidates);
  this->ref.inliers_num = 0;
  inliers_index = this->ref.pupil_fitting_inliers((UINT8*)this->eye_image->imageData, 
																								 this->eye_image->width, this->eye_image->height, 
																								 this->ref.inliers_num);
  ellipse_axis.width = (int)this->ref.pupil_param[0];
  ellipse_axis.height = (int)this->ref.pupil_param[1];
  this->pupil.x = (int)this->ref.pupil_param[2];
  this->pupil.y = (int)this->ref.pupil_param[3];
  this->Draw_Cross(this->ellipse_image, 
									this->pupil.x, this->pupil.y, 
									15, 15, Red);
  cvLine(this->eye_image, this->pupil, this->corneal_reflection, Red, 4, 8);
  cvLine(this->ellipse_image, this->pupil, this->corneal_reflection, Red, 4, 8);
  
  printf("ellipse a:%lf; b:%lf, cx:%lf, cy:%lf, theta:%lf; inliers_num:%d\n\n", 
         this->ref.pupil_param[0], this->ref.pupil_param[1], this->ref.pupil_param[2], 
				 this->ref.pupil_param[3], this->ref.pupil_param[4], this->ref.inliers_num);

  bool is_inliers = 0;
  for (i = 0; i < this->ref.edge_point.size(); i++) {
    is_inliers = 0;
    for (j = 0; j < this->ref.inliers_num; j++) {
      if (i == inliers_index[j])
        is_inliers = 1;
    }
    stuDPoint *edge = this->ref.edge_point.at(i);
    if (is_inliers)
      this->Draw_Cross(this->ellipse_image, (int)edge->x,(int)edge->y, 5, 5, Green);
    else
      this->Draw_Cross(this->ellipse_image, (int)edge->x,(int)edge->y, 3, 3, Yellow);
  }
  free(inliers_index);

  if (ellipse_axis.width > 0 && ellipse_axis.height > 0) {
    this->ref.start_point.x = this->pupil.x;
    this->ref.start_point.y = this->pupil.y;
    //printf("start_point: %d,%d\n", start_point.x, start_point.y);
    this->Draw_Cross(this->eye_image, this->pupil.x, this->pupil.y, 10, 10, Green);
    cvEllipse(this->eye_image, this->pupil, ellipse_axis, -this->ref.pupil_param[4]*180/PI, 0, 360, Red, 2);
    cvEllipse(this->ellipse_image, this->pupil, ellipse_axis, -this->ref.pupil_param[4]*180/PI, 0, 360, Green, 2);

    this->diff_vector.x = this->pupil.x - this->corneal_reflection.x;
    this->diff_vector.y = this->pupil.y - this->corneal_reflection.y;
    lost_frame_num = 0;    
  } else {
    lost_frame_num++;
  }
  if (lost_frame_num > 5) {
    this->ref.start_point.x = this->width/2;
    this->ref.start_point.y = this->height/2;
  }
  this->Draw_Cross(this->ellipse_image, 
								  (int)this->ref.start_point.x, (int)this->ref.start_point.y, 
									7, 7, Blue);
  this->Draw_Cross(this->eye_image, 
									(int)this->ref.start_point.x, (int)this->ref.start_point.y, 
									7, 7, Blue);
}


void cvEyeTracker::Draw_Cross(IplImage *image, int centerx, int centery, int x_cross_length, int y_cross_length, CvScalar color)
{
  CvPoint pt1,pt2,pt3,pt4;

  pt1.x = centerx - x_cross_length;
  pt1.y = centery;
  pt2.x = centerx + x_cross_length;
  pt2.y = centery;

  pt3.x = centerx;
  pt3.y = centery - y_cross_length;
  pt4.x = centerx;
  pt4.y = centery + y_cross_length;

  cvLine(image,pt1,pt2,color,1,8);
  cvLine(image,pt3,pt4,color,1,8);
}


void cvEyeTracker::Calculate_Avg_Intensity_Hori(IplImage* in_image)
{
  UINT8 *pixel = (UINT8*)in_image->imageData;
  int sum;
  int i, j;
  for (j = 0; j < in_image->height; j++) {
    sum = 0;
    for (i = 0; i < in_image->width; i++) {
      sum += *pixel;
      pixel++;
    }
    this->avg_intensity_hori[j] = (double)sum/in_image->width;
  }
}


void cvEyeTracker::Reduce_Line_Noise(IplImage* in_image)
{
  UINT8 *pixel = (UINT8*)in_image->imageData;
  int i, j;
  double beta2 = 1 - beta;
  int adjustment;

  this->Calculate_Avg_Intensity_Hori(in_image);
  for (j = 0; j < in_image->height; j++) {
    this->intensity_factor_hori[j] = this->avg_intensity_hori[j]*beta + this->intensity_factor_hori[j]*beta2;
    adjustment = (int)(this->intensity_factor_hori[j] - this->avg_intensity_hori[j]);
    for (i = 0; i < in_image->width; i++) {
      *pixel =  FIX_UINT8(*pixel+adjustment);
      pixel++;
    }
  }
}