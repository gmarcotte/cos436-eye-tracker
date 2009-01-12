/* This header file provides an interface to access the main eye tracking
   functionality of the cvEyeTracker software as a library, for incorporation
	 into other applications.  The original license and copyright information
	 for cvEyeTracker is reproduced below.
*/

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

#include <cv.h>
#include "RansacEllipseFinder.h"


class cvEyeTracker {
public:
	// Parameters that clients can update on the fly
	int min_pupil_contour_points;   
	int max_pupil_contour_points;    
	int pupil_size_tolerance;	//range of allowed pupil diameters
	int max_contour_count;

	//parameters for the algorithm
	int edge_threshold;					//threshold of pupil edge points detection
	int rays;										//number of rays to use to detect feature points
	int min_feature_candidates;	//minimum number of pupil feature candidates
	int cr_window_size;					//corneal refelction search window size

private:
	// State variables that are not user-accessible
	IplImage *eye_image;
	IplImage *threshold_image;
	IplImage *ellipse_image;

	CvPoint pupil;								//coordinates of pupil in tracker coordinate system
	CvPoint corneal_reflection;		//coordinates of corneal reflection in tracker coordinate system
	CvPoint diff_vector;					//vector between the corneal reflection and pupil
	int corneal_reflection_r;					//the radius of corneal reflection

	int width;
	int height;

	double beta;									//hysteresis factor for noise reduction
	double *intensity_factor_hori;			//horizontal intensity factor for noise reduction
	double *avg_intensity_hori;					//horizontal average intensity

	RansacEllipseFinder ref;


public:
	// Constructor - requires the frame size
	cvEyeTracker(int w, int h);
	
	// Destructor
	~cvEyeTracker();
	
	// Private state accessors
	CvPoint getPupilCoordinates();
	CvPoint getCornealReflectionCoordinates();
	CvPoint getDifferenceVector();
	int getCornealReflectionRadius();

	void getEyeImage(IplImage* im);
	void getEllipseImage(IplImage* im);
	void getThresholdImage(IplImage* im);

	// Processing methods related to actually performing the tracking algorithm
	void process(IplImage* original_eye);

private:
	void Draw_Cross(IplImage *image, int centerx, int centery, int x_cross_length, int y_cross_length, CvScalar color);
	void Calculate_Avg_Intensity_Hori(IplImage* in_image);
	void Reduce_Line_Noise(IplImage* in_image);

};