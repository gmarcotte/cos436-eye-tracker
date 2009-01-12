#include <stdio.h>
#include <math.h>

#include <cv.h>
#include <highgui.h>

#include "CaptureHandler.h"
#include "GUI.h"
#include "config.h"

void invertImage(IplImage* img)
{
	// get the image data
	int i, j, k;
  int height    = img->height;
  int width     = img->width;
  int step      = img->widthStep;
  int channels  = img->nChannels;
  uchar *data      = (uchar *)img->imageData;
  for(i=0;i<height;i++) for(j=0;j<width;j++) for(k=0;k<channels;k++)
    data[i*step+j*channels+k]=255-data[i*step+j*channels+k];
}

double eccentricity(CvBox2D ellipse)
{
	double a = ellipse.size.height / 2;
	double b = ellipse.size.width / 2;
	if (a <= b)
		return sqrt(1 - (a / b)*(a / b));
	else
		return sqrt(1 - (b / a)*(b / a));
}

double dist(CvPoint2D32f a, CvPoint2D32f b)
{
	return sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) );
}

double norm(CvPoint2D32f a)
{
	return dist(cvPoint2D32f(0.0, 0.0), a);
}


int main(int argc, char* argv[])
{
	int i;
	CaptureHandler* captures[NUM_INPUTS];
	int index;
	for (i=0, index=0; index<NUM_INPUTS; i++)
	{
		if (i >= argc - 1 + NUM_INPUTS)
		{
			// If we've expended all command line arguments as well as camera numbers
			// 0 to NUM_INPUTS - 1, then free up memory and quit with an error
			printf("Error: could only find %d out of %d required inputs, exiting!\n", index, NUM_INPUTS);
			int j;
			for (j=0; j<index; j++)
				delete captures[j];
			return 0;
		}
		if (i + 1 < argc)
		{
			// There are command-line arguments left, so try to load video
			try {
				captures[index] = new CaptureHandler(argv[i+1]);
			} catch(CaptureHandler::FileReadError) {
				printf("Read Error: skipping file %s\n", argv[i+1]);
				continue;
			}
			printf("Eye %d Tracking from %s\n", index, argv[i+1]);
			captures[index]->printFullState();
			printf("\n\n");
			index++;
		}
		else
		{
			// There are no command-line arguments left, so try to load a camera
			try {
				captures[index] = new CaptureHandler(i + 1 - argc);
			} catch(CaptureHandler::CameraReadError) {
				printf("Not Found: skipping camera %d\n", i + 1 - argc);
				continue;
			}
			printf("Eye %d Tracking from Camera %d\n", index, i + 1 - argc);
			captures[index]->printFullState();
			index++;
		}
	}
	// Algorithm Configuration Parameters
	char* eyebrow_threshold_trackbar = "EB Thresh";
	char* eye_threshold_trackbar = "E Thresh";
	char* pupil_threshold_trackbar = "P Thresh";
	char* intensity_threshold_trackbar = "Threshold";
	const double intensity_threshold_init = 120;
	const double intensity_threshold_low = 0;
	const double intensity_threshold_high = 255;
	const int intensity_threshold_resolution = 255;

	char* min_eyebrow_length_trackbar = "Min EB Len";
	char* max_eyebrow_length_trackbar = "Max EB Len";
	char* min_eye_length_trackbar = "Min E Len";
	char* max_eye_length_trackbar = "Max E Leng";
	char* min_pupil_length_trackbar = "Min P Len";
	char* contour_filter_trackbar = "Contour Filter";
	const double contour_filter_init = 50;
	const double contour_filter_min = 0;
	const double contour_filter_max = 1000;
	const int contour_filter_resolution = 2000;

	
	char* min_eyebrow_eccentricity_trackbar = "Min EB Ecc";
	char* min_eye_eccentricity_trackbar = "Min E Ecc";
	char* max_eye_eccentricity_trackbar = "Max E Ecc";
	char* max_pupil_eccentricity_trackbar = "Max P Ecc";
	char* eccentricity_filter_trackbar = "Eccentricity Filter";
	const double eccentricity_init = 50;
	const double eccentricity_min = 0;
	const double eccentricity_max = 1;
	const int eccentricity_resolution = 100;

	// Set up the configuration GUI
	char* config_name = "EyeTracker Configuration";
	cvNamedWindow(config_name, 0);

	ConfigTrackbar* eyebrow_threshold = new ConfigTrackbar(
		eyebrow_threshold_trackbar, config_name,
		intensity_threshold_init, intensity_threshold_low, 
		intensity_threshold_high, intensity_threshold_resolution);

	ConfigTrackbar* min_eyebrow_length = new ConfigTrackbar(
		min_eyebrow_length_trackbar, config_name,
		contour_filter_init, contour_filter_min,
		contour_filter_max, contour_filter_resolution);

	ConfigTrackbar* max_eyebrow_length = new ConfigTrackbar(
		max_eyebrow_length_trackbar, config_name,
		contour_filter_init, contour_filter_min,
		contour_filter_max, contour_filter_resolution);

	ConfigTrackbar* min_eyebrow_eccentricity = new ConfigTrackbar(
		min_eyebrow_eccentricity_trackbar, config_name,
		eccentricity_init, eccentricity_min, eccentricity_max,
		eccentricity_resolution);

	ConfigTrackbar* eye_threshold = new ConfigTrackbar(
		eye_threshold_trackbar, config_name,
		intensity_threshold_init, intensity_threshold_low, 
		intensity_threshold_high, intensity_threshold_resolution);

	ConfigTrackbar* min_eye_length = new ConfigTrackbar(
		min_eye_length_trackbar, config_name,
		contour_filter_init, contour_filter_min,
		contour_filter_max, contour_filter_resolution);

	ConfigTrackbar* max_eye_length = new ConfigTrackbar(
		max_eye_length_trackbar, config_name,
		contour_filter_init, contour_filter_min,
		contour_filter_max, contour_filter_resolution);

	ConfigTrackbar* min_eye_eccentricity = new ConfigTrackbar(
		min_eye_eccentricity_trackbar, config_name,
		eccentricity_init, eccentricity_min, eccentricity_max,
		eccentricity_resolution);

	ConfigTrackbar* max_eye_eccentricity = new ConfigTrackbar(
		max_eye_eccentricity_trackbar, config_name,
		eccentricity_init, eccentricity_min, eccentricity_max,
		eccentricity_resolution);

	ConfigTrackbar* pupil_threshold = new ConfigTrackbar(
		pupil_threshold_trackbar, config_name,
		intensity_threshold_init, intensity_threshold_low, 
		intensity_threshold_high, intensity_threshold_resolution);

	ConfigTrackbar* min_pupil_length = new ConfigTrackbar(
		min_pupil_length_trackbar, config_name,
		contour_filter_init, contour_filter_min,
		contour_filter_max, contour_filter_resolution);

	ConfigTrackbar* max_pupil_eccentricity = new ConfigTrackbar(
		max_pupil_eccentricity_trackbar, config_name,
		eccentricity_init, eccentricity_min, eccentricity_max,
		eccentricity_resolution);


	// Set up the eye trackers, image memory and display windows for each input
	IplImage* original_eyes[NUM_INPUTS];
	IplImage* modified_eyes[NUM_INPUTS];
	IplImage* contour_eyes[NUM_INPUTS];
	IplImage* gray_eyes[NUM_INPUTS];

	char original_names[NUM_INPUTS][MAX_STRING];
	char modified_names[NUM_INPUTS][MAX_STRING];
	char inverted_names[NUM_INPUTS][MAX_STRING];
	char thresholded_names[NUM_INPUTS][MAX_STRING];
	char contour_names[NUM_INPUTS][MAX_STRING];
	char pupil_names[NUM_INPUTS][MAX_STRING];
	char eyebrow_names[NUM_INPUTS][MAX_STRING];
	char eye_names[NUM_INPUTS][MAX_STRING];
	for (i=0; i<NUM_INPUTS; i++)
	{
		sprintf_s(original_names[i], MAX_STRING, "Input %d: Direct Feed", i);
		sprintf_s(modified_names[i], MAX_STRING, "Input %d: Modified Eye Image", i);
		sprintf_s(inverted_names[i], MAX_STRING, "Input %d: Inverted Eye Image", i);
		sprintf_s(thresholded_names[i], MAX_STRING, "Input %d: Thresholded Eye Image", i);
		sprintf_s(contour_names[i], MAX_STRING, "Input %d: Eye Image Contours", i);
		sprintf_s(eyebrow_names[i], MAX_STRING, "Input %d: Thresholded Eyebrow", i);
		sprintf_s(eye_names[i], MAX_STRING, "Input %d: Thresholded Eye", i);
		sprintf_s(pupil_names[i], MAX_STRING, "Input %d: Thresholded Pupil", i);
		captures[i]->openInWindow(original_names[i]);
		cvNamedWindow(modified_names[i]);
		cvNamedWindow(thresholded_names[i]);
		cvNamedWindow(inverted_names[i]);
		cvNamedWindow(contour_names[i]);
		cvNamedWindow(eyebrow_names[i]);
		cvNamedWindow(eye_names[i]);
		cvNamedWindow(pupil_names[i]);

		original_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																		 IPL_DEPTH_8U, 3);
		modified_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																		 IPL_DEPTH_8U, 1);
		contour_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																		 IPL_DEPTH_8U, 1);
		gray_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																 IPL_DEPTH_8U, 1);
	}

	CvMemStorage* storage = cvCreateMemStorage(0);

	char c;
	//CvBox2D best_pupil;
	//CvBox2D best_eyebrow;
	//CvBox2D best_eye;
	while ( (c = cvWaitKey(33)) != 'q')
	{
		for (i = 0; i < NUM_INPUTS; i++)
		{
			captures[i]->advance();
			cvZero(original_eyes[i]);
			captures[i]->getCurrentFrame(original_eyes[i]);
			captures[i]->getCurrentFrame(gray_eyes[i]);
			cvSmooth(gray_eyes[i], gray_eyes[i], CV_GAUSSIAN, 5, 5);
			CvSeq* contours = 0;
			CvSeq* curr_contour;
			/*
			cvFindContours(modified_eyes[i], storage, &contours);
			cvZero(contour_eyes[i]);
			CvSeq* curr_contour = contours;
			while (curr_contour != NULL)
			{
				//printf("Contour Length: %6.2f\n", cvContourPerimeter(curr_contour));
				cvDrawContours(contour_eyes[i], curr_contour, cvScalarAll(255), cvScalarAll(255), 1);
				CvBox2D box = cvMinAreaRect2(curr_contour);
				cvDrawEllipse(contour_eyes[i], 
											cvPoint(box.center.x, box.center.y), 
											cvSize(box.size.height / 2, box.size.width / 2), 
											box.angle, 0.0, 360.0, cvScalarAll(255));
				curr_contour = curr_contour->h_next;
			}
			*/

			int run_once;

			/***********************************************************************************/
			// Find the best guess at the eye
			cvZero(modified_eyes[i]);
			cvThreshold(gray_eyes[i], modified_eyes[i], eye_threshold->getValue(), 255, CV_THRESH_BINARY_INV);
			cvShowImage(eye_names[i], modified_eyes[i]);
			cvClearMemStorage(storage);
			contours = 0;
			cvFindContours(modified_eyes[i], storage, &contours);
			curr_contour = contours;

			run_once = 0;
			CvBox2D best_eye;
			best_eye.center.x = -1;
			curr_contour = contours;
			while (curr_contour != NULL)
			{
				CvBox2D box = cvMinAreaRect2(curr_contour);
				if (cvContourPerimeter(curr_contour) >= min_eye_length->getValue() &&
						cvContourPerimeter(curr_contour) <= max_eye_length->getValue() &&
						eccentricity(box) >= min_eye_eccentricity->getValue() &&
						eccentricity(box) <= max_eye_eccentricity->getValue())
				{
					if (run_once == 0)
					{
						best_eye = box;
						run_once = 1;
					}
					else if ( (box.size.height * box.size.width) > (best_eye.size.height * best_eye.size.width))
						best_eye = box;
				}
				curr_contour = curr_contour->h_next;
			}

			//best_eyebrow = curr_best_eyebrow;
			
			
			if ( (best_eye.center.x >= 0 && best_eye.center.x <= captures[i]->getWidth()) &&
					 (best_eye.center.y >= 0 && best_eye.center.y <= captures[i]->getHeight()) )
			{
				printf("Best Eye: \n\tPos: %6.2f, %6.2f, \n\tSize: %6.2f, %6.2f, \n\tAngle: %6.2f, Ecc: %6.2f\n", 
							  best_eye.center.x, best_eye.center.y, 
							  best_eye.size.height, best_eye.size.height,
							  best_eye.angle, eccentricity(best_eye));
				cvDrawEllipse(original_eyes[i],
											cvPoint(best_eye.center.x, best_eye.center.y), 
											cvSize(best_eye.size.height / 2, best_eye.size.width / 2), 
											best_eye.angle, 0.0, 360.0, CV_RGB(0, 0, 255), 2);
			}
			else
			{
				printf("No eye detected.\n");
			}
			/**********************************************************************************/

			/***********************************************************************************/
			// Find the best guess at the pupil
			if (best_eye.center.x != -1)
			{
				cvZero(modified_eyes[i]);
				cvThreshold(gray_eyes[i], modified_eyes[i], pupil_threshold->getValue(), 255, CV_THRESH_BINARY_INV);
				cvShowImage(pupil_names[i], modified_eyes[i]);
				cvClearMemStorage(storage);
				contours = 0;
				cvFindContours(modified_eyes[i], storage, &contours);
				curr_contour = contours;

				run_once = 0;
				CvBox2D best_pupil;
				best_pupil.center.x = -1;
				curr_contour = contours;
				while (curr_contour != NULL)
				{
					CvBox2D box = cvMinAreaRect2(curr_contour);
					if  (eccentricity(box) <= max_pupil_eccentricity->getValue() &&
							 cvContourPerimeter(curr_contour) > min_pupil_length->getValue())
					{
						if (run_once == 0)
						{
							best_pupil = box;
							run_once = 1;
						}
						// Find the most circular contour that is within the eye estimate
						else if (eccentricity(box) < eccentricity(best_pupil) &&
										 dist(box.center, best_eye.center) < max(best_eye.size.height, best_eye.size.width) )
							best_pupil = box;
					}
					curr_contour = curr_contour->h_next;
				}

				//best_eyebrow = curr_best_eyebrow;
				if ( (best_pupil.center.x >= 0 && best_pupil.center.x <= captures[i]->getWidth()) &&
						 (best_pupil.center.y >= 0 && best_pupil.center.y <= captures[i]->getHeight()) )
				{
					printf("Best Pupil: \n\tPos: %6.2f, %6.2f, \n\tSize: %6.2f, %6.2f, \n\tAngle: %6.2f, Ecc: %6.2f\n", 
								 best_pupil.center.x, best_pupil.center.y, 
								 best_pupil.size.height, best_pupil.size.height,
								 best_pupil.angle, eccentricity(best_pupil));
					cvDrawEllipse(original_eyes[i], 
												cvPoint(best_pupil.center.x, best_pupil.center.y), 
												cvSize(best_pupil.size.height / 2, best_pupil.size.width / 2), 
												best_pupil.angle, 0.0, 360.0, CV_RGB(0, 255, 0), 2);
				}
				else
				{
					printf("No Pupil Detected.\n");
				}
			}
			else
			{
				printf("No Pupil Detected, because no eye could be found.\n");
			}
			/***********************************************************************************/

			/***********************************************************************************/
			// Find the best guess at the eyebrow
			cvZero(modified_eyes[i]);
			cvThreshold(gray_eyes[i], modified_eyes[i], eyebrow_threshold->getValue(), 255, CV_THRESH_BINARY_INV);
			cvShowImage(eyebrow_names[i], modified_eyes[i]);
			cvClearMemStorage(storage);
			contours = 0;
			cvFindContours(modified_eyes[i], storage, &contours);
			curr_contour = contours;

			run_once = 0;
			CvBox2D best_eyebrow;
			best_eyebrow.center.x = -1;
			curr_contour = contours;
			while (curr_contour != NULL)
			{
				CvBox2D box = cvMinAreaRect2(curr_contour);
				if (cvContourPerimeter(curr_contour) >= min_eyebrow_length->getValue() &&
						cvContourPerimeter(curr_contour) <= max_eyebrow_length->getValue() &&
						eccentricity(box) >= min_eyebrow_eccentricity->getValue() )
				{
					if (run_once == 0)
					{
						best_eyebrow = box;
						run_once = 1;
					}
					else if (best_eyebrow.center.y > box.center.y)
						best_eyebrow = box;
				}
				curr_contour = curr_contour->h_next;
			}

			//best_eyebrow = curr_best_eyebrow;
			
			
			if ( (best_eyebrow.center.x >= 0 && best_eyebrow.center.x <= captures[i]->getWidth()) &&
					 (best_eyebrow.center.y >= 0 && best_eyebrow.center.y <= captures[i]->getHeight()) )
			{
				printf("Best Eyebrow: \n\tPos: %6.2f, %6.2f, \n\tSize: %6.2f, %6.2f, \n\tAngle: %6.2f, Ecc: %6.2f\n", 
							  best_eyebrow.center.x, best_eyebrow.center.y, 
							  best_eyebrow.size.height, best_eyebrow.size.height,
							  best_eyebrow.angle, eccentricity(best_eyebrow));
				cvDrawEllipse(original_eyes[i], 
											cvPoint(best_eyebrow.center.x, best_eyebrow.center.y), 
											cvSize(best_eyebrow.size.height / 2, best_eyebrow.size.width / 2), 
											best_eyebrow.angle, 0.0, 360.0, CV_RGB(255, 0, 0), 2);
			}
			else
			{
				printf("No eyebrow detected.\n");
			}
			/**********************************************************************************/

			cvShowImage(modified_names[i], original_eyes[i]);
			cvShowImage(contour_names[i], contour_eyes[i]);
		}
	}


	// Free memory allocated for input captures
	for (i=0; i<NUM_INPUTS; i++)
	{
		cvReleaseImage(&original_eyes[i]);
		cvReleaseImage(&modified_eyes[i]);
		cvReleaseImage(&contour_eyes[i]);
		cvReleaseImage(&gray_eyes[i]);
		cvDestroyWindow(modified_names[i]);
		cvDestroyWindow(thresholded_names[i]);
		cvDestroyWindow(inverted_names[i]);
		cvDestroyWindow(contour_names[i]);
		cvDestroyWindow(eyebrow_names[i]);
		cvDestroyWindow(eye_names[i]);
		cvDestroyWindow(pupil_names[i]);
		delete captures[i];
		captures[i] = NULL;
	}

	// Free the configuration GUI
	delete eye_threshold;
	delete eyebrow_threshold;
	delete pupil_threshold;
	delete min_eyebrow_length;
	delete max_eyebrow_length;
	delete min_eyebrow_eccentricity;
	delete min_eye_length;
	delete max_eye_length;
	delete min_eye_eccentricity;
	delete max_eye_eccentricity;
	delete max_pupil_eccentricity;
	delete min_pupil_length;
	cvDestroyWindow(config_name);

	return 1;
}