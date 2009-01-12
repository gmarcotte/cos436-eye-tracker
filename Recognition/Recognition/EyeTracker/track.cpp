#include <stdio.h>

#include <cv.h>
#include <highgui.h>
#include <math.h>

#include "..\Recognizer.h"
#include "..\Eye.h"
#include "CaptureHandler.h"
#include "config.h"
#include "GUI.h"

#ifndef PI
#define PI 3.1415926
#endif


class Pupil {
public:
	int x;
	int y;
	int radius;
};

double dist(CvPoint a, CvPoint b)
{
	return sqrt( (double)((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)) );
}

double dist(CvPoint2D32f a, CvPoint2D32f b)
{
	return sqrt( (double)((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)) );
}

double heron(double a, double b, double c)
{
	double s = (a + b + c) / 2;
	return sqrt(s * (s - a) * (s - b) * (s - c));
}

bool boxContainsCircle(CvPoint2D32f* box, CvPoint2D32f center, double radius)
{
	double a, b;
	double alt;

	double width = dist(box[0], box[1]);
	double height = dist(box[1], box[2]);

	a = dist(box[0], center);
	b = dist(box[1], center);

	// Check that the center is contained in the box
	if (acos( (width*width + b*b - a*a) / (2 * width * b) ) > PI/2)
		return false;
	if (acos( (width*width + a*a - b*b) / (2 * width * a) ) > PI/2)
		return false;

	// Now check the distance from the center to the width side
	alt = 2 * heron(a, b, width) / width;
	if (alt < radius || (height - alt) < radius)
		return false;


	// Check that the center is contained in the box
	a = dist(box[2], center);
	if (acos( (height*height + b*b - a*a) / (2 * height * b) ) > PI/2)
		return false;
	if (acos( (height*height + a*a - b*b) / (2 * height * a) ) > PI/2)
		return false;

	// Now check the distance from the center to the height side
	alt = 2 * heron(a, b, height) / height;
	if (alt < radius || (width - alt) < radius)
		return false;

	return true;
}

bool boxContainsCircle(CvRect box, CvPoint center, int radius)
{
	return ( ((center.x - radius) >= box.x) &&
					 ((center.y - radius) >= box.y) &&
					 ((center.x + radius) <= box.x + box.width) &&
					 ((center.y + radius) <= box.y + box.height) );
}

void updatePupil(IplImage* inputImg, Pupil* update_pup)
{
	CvMemStorage* storage = cvCreateMemStorage(1000);
	CvSeq* firstContour;
	int headerSize = sizeof(CvContour);

	int count;
	CvPoint* pointArray;
	CvPoint2D32f* pointArray32f;
	int i;

	CvBox2D* myBox = (CvBox2D*)malloc(sizeof(CvBox2D));
	CvPoint myCenter;
	int height, width;

	update_pup->radius = 0;
	
	//	**	Contours are found
	cvFindContours(inputImg, storage, &firstContour, headerSize, 
				   CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1 );

	//cvNamedWindow("draw", 0);
	//	**	Search for valid contours
	int max = 0;
	while (firstContour != NULL)
	{
		//cvDrawContours(inputImg, firstContour,RGB(0,0,255),CV_FILLED,0);
		//cvShowImage("draw", inputImg);
		//cvWaitKey(0);
		
		//	not a point?
		if ( CV_IS_SEQ_CURVE(firstContour) )
		{
			count = firstContour->total;

			pointArray = (CvPoint*) malloc(count*sizeof(CvPoint));
			pointArray32f = (CvPoint2D32f*) malloc((count+1)*sizeof(CvPoint2D32f));

			//	**	Get contour points
			cvCvtSeqToArray(firstContour, pointArray, CV_WHOLE_SEQ);
			//	**	Convert to 32f points
			for (i=0; i<count; i++)
			{
				pointArray32f[i].x = (float)pointArray[i].x;
				pointArray32f[i].y = (float)pointArray[i].y;
			}
			pointArray32f[i].x = pointArray[0].x;
			pointArray32f[i].y = pointArray[0].y;

			if (count>=6)
			{
	
				//	**	Fit Ellipse to the points
				cvFitEllipse(pointArray32f, count, myBox);

				myCenter.x = (int)myBox->center.x;
				myCenter.y = (int)myBox->center.y;
				height	= (int)myBox->size.height;
				width	= (int)myBox->size.width;
				
				//cvCircle(inputImg,myCenter, (int)length/2 ,RGB(0,0,255));
				//float myAngle= myBox->angle;
				//cvEllipse(inputImg, myCenter, cvSize ((int)width/2,(int)height/2), -myBox->angle, 0,360,RGB(0,255,0),1);
				
				//	**	Check whether it is a valid connected component or not?
				if ( (myCenter.x > 0) && (myCenter.y > 0) && 
					 ((height+width) > max) && ((height-width) <= width) && 
					 ((width-height) <= height) && (width <= (int)inputImg->width/2) && (height <= (int)inputImg->height/2))
				{
					max = height + width;
					update_pup->x = myCenter.x;
					update_pup->y = myCenter.y;
					height>width ? update_pup->radius=(int)height/2 : update_pup->radius=(int)width/2;
				}
			}
			
			free(pointArray);
			free(pointArray32f);
		}
		//cvShowImage("draw", inputImg); cvWaitKey(0);
		firstContour = firstContour->h_next;	
	}

	free(myBox);
	cvReleaseMemStorage(&storage);
	
	//cvDestroyWindow("draw");
}

void updateEyebrow(IplImage* img, CvBox2D* eyebrow, Pupil* pupil)
{
	CvSeq* contours;
	IplImage* scratch = cvCloneImage(img);
	CvMemStorage* storage = cvCreateMemStorage(0);
	cvFindContours(scratch, storage, &contours);
	CvSeq* curr_contour = contours;

	int run_once = 0;
	CvBox2D best_eyebrow;
	best_eyebrow.center.x = -1;
	while (curr_contour != NULL)
	{
		CvBox2D box = cvMinAreaRect2(curr_contour);
		if ( cvContourPerimeter(curr_contour) >= MIN_EB_PERIM &&
				 (pupil->radius == 0 ||
				  (abs(box.center.x - pupil->x) < 200 &&
				   (pupil->y - box.center.y) > 10) ))
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
	*(eyebrow) = best_eyebrow;
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&scratch);
}




void updateEye(IplImage* img, CvRect* eye, Pupil* pupil, CvBox2D* eyebrow)
{
	CvSeq* contours;
	IplImage* scratch = cvCloneImage(img);
	CvMemStorage* storage = cvCreateMemStorage(0);
	cvFindContours(scratch, storage, &contours);
	CvSeq* curr_contour = contours;

	int run_once = 0;
	CvRect best_eye;
	best_eye.x = -1;
	curr_contour = contours;
	while (curr_contour != NULL)
	{
		CvRect box = cvBoundingRect(curr_contour);
		if ( ((eyebrow->center.x != -1 ||
					 ( abs(box.x - eyebrow->center.x) < 100 &&
					   box.y - eyebrow->center.y > 0) ) &&
			    (pupil->radius == 0 ||
					 boxContainsCircle(box, cvPoint(pupil->x, pupil->y), pupil->radius))) )
		{
			if (run_once == 0)
			{
				best_eye = box;
				run_once = 1;
			}
			else if ( (box.height * box.width) > (best_eye.height * best_eye.width))
				best_eye = box;
		}
		curr_contour = curr_contour->h_next;
	}
	*(eye) = best_eye;
	cvReleaseImage(&scratch);
	cvReleaseMemStorage(&storage);
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

	// Set up the eye trackers, image memory and display windows for each input
	IplImage* original_eyes[NUM_INPUTS];
	IplImage* modified_eyes[NUM_INPUTS];
	IplImage* threshold_eyes[NUM_INPUTS];

	char original_names[NUM_INPUTS][MAX_STRING];
	char modified_names[NUM_INPUTS][MAX_STRING];
	char eyebrow_names[NUM_INPUTS][MAX_STRING];
	char pupil_names[NUM_INPUTS][MAX_STRING];
	char eye_names[NUM_INPUTS][MAX_STRING];
	for (i=0; i<NUM_INPUTS; i++)
	{
		sprintf_s(original_names[i], MAX_STRING, "Input %d: Direct Feed", i);
		sprintf_s(modified_names[i], MAX_STRING, "Input %d: Modified Eye Image", i);
		sprintf_s(eyebrow_names[i], MAX_STRING, "Input %d: Thresholded Eyebrow", i);
		sprintf_s(eye_names[i], MAX_STRING, "Input %d: Thresholded Eye", i);
		sprintf_s(pupil_names[i], MAX_STRING, "Input %d: Thresholded Pupil", i);
		captures[i]->openInWindow(original_names[i]);
		cvNamedWindow(modified_names[i]);
		cvNamedWindow(eyebrow_names[i]);
		cvNamedWindow(eye_names[i]);
		cvNamedWindow(pupil_names[i]);

		original_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																		 IPL_DEPTH_8U, 3);
		modified_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																		 IPL_DEPTH_8U, 1);
		threshold_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
						    										  IPL_DEPTH_8U, 1);
	}

	// Set up the config GUI
	char* config_name = "EyeTracker Configuration";
	cvNamedWindow(config_name, 0);

	ConfigTrackbar* pupil_threshold = new ConfigTrackbar(
		"P Thresh", config_name, 80, 0, 255, 255);

	ConfigTrackbar* canny_high = new ConfigTrackbar(
		"Canny Hi", config_name, 250, 0, 500, 500);

	ConfigTrackbar* canny_low = new ConfigTrackbar(
		"Canny Low", config_name, 200, 0, 500, 500);

	ConfigTrackbar* eyebrow_threshold = new ConfigTrackbar(
		"EB Thresh", config_name, 100, 0, 255, 255);

	ConfigTrackbar* eye_threshold = new ConfigTrackbar(
		"E Thresh", config_name, 120, 0, 255, 255);

	Recognizer* recog = new Recognizer();

	Eye* eye_trackers[NUM_INPUTS];
	Pupil pupils[NUM_INPUTS];
	CvBox2D eyebrows[NUM_INPUTS];
	CvRect eyes[NUM_INPUTS];
	for (i = 0; i < NUM_INPUTS; i++)
	{
		eye_trackers[i] = new Eye();

		pupils[i].x = 0;
		pupils[i].y = 0;
		pupils[i].radius = 0;

		eyebrows[i].angle = 0;
		eyebrows[i].center = cvPoint2D32f(0, 0);
		eyebrows[i].size = cvSize2D32f(0, 0);

		eyes[i] = cvRect(0, 0, 0, 0);
	}
	
	bool send_data = false;
	char c;
	// Quit with Escape
	while ( (c = cvWaitKey(33)) != 27)
	{
		// Start and stop recognition with Spacebar
		if (c == 32)
			send_data = !send_data;

		for (i = 0; i < NUM_INPUTS; i++)
		{
			captures[i]->advance();
			captures[i]->getCurrentFrame(original_eyes[i], FLIP_INPUT);
			cvZero(modified_eyes[i]);
			cvCvtPixToPlane(original_eyes[i], NULL, NULL, modified_eyes[i], NULL );
			cvSmooth(modified_eyes[i], modified_eyes[i], CV_GAUSSIAN, 5, 5);

			// Pupil detection
			cvThreshold(modified_eyes[i], threshold_eyes[i], pupil_threshold->getValue(), 255, CV_THRESH_BINARY_INV);
			cvCanny(threshold_eyes[i], threshold_eyes[i], canny_low->getValue(), canny_high->getValue(), 3);
			cvShowImage(pupil_names[i], threshold_eyes[i]);
			updatePupil(threshold_eyes[i], &pupils[i]);
			//printf("Found pupil %d: (%d, %d) - %d\n", i, pupils[i].x, pupils[i].y, pupils[i].radius);
			if (pupils[i].radius > 0)
				cvDrawCircle(original_eyes[i], cvPoint(pupils[i].x, pupils[i].y), pupils[i].radius, CV_RGB(255, 0, 0), 2);


			// Eyebrow Detection
			cvThreshold(modified_eyes[i], threshold_eyes[i], eyebrow_threshold->getValue(), 255, CV_THRESH_BINARY_INV);
			cvShowImage(eyebrow_names[i], threshold_eyes[i]);
			updateEyebrow(threshold_eyes[i], &eyebrows[i], &pupils[i]);		
			if ( (eyebrows[i].center.x >= 0 && eyebrows[i].center.x <= captures[i]->getWidth()) &&
					 (eyebrows[i].center.y >= 0 && eyebrows[i].center.y <= captures[i]->getHeight()) )
			{
				//printf("Found Eyebrow: \n\tPos: %6.2f, %6.2f, \n\tSize: %6.2f, %6.2f, \n\tAngle: %6.2f\n", 
				//			  eyebrows[i].center.x, eyebrows[i].center.y, 
				//			  eyebrows[i].size.width, eyebrows[i].size.height,
				//			  eyebrows[i].angle);
				cvDrawEllipse(original_eyes[i], 
											cvPoint(eyebrows[i].center.x, eyebrows[i].center.y), 
											cvSize(eyebrows[i].size.height / 2, eyebrows[i].size.width / 2), 
											eyebrows[i].angle, 0.0, 360.0, CV_RGB(0, 255, 0), 2);
			}
			//else
			//{
			//	printf("No eyebrow detected.\n");
			//}
			

			// Eye Detection
			cvThreshold(modified_eyes[i], threshold_eyes[i], eye_threshold->getValue(), 255, CV_THRESH_BINARY_INV);
			cvShowImage(eye_names[i], threshold_eyes[i]);
			updateEye(threshold_eyes[i], &eyes[i], &pupils[i], &eyebrows[i]);	
			if ( (eyes[i].x >= 0 && eyes[i].x <= captures[i]->getWidth()) &&
					 (eyes[i].y >= 0 && eyes[i].y <= captures[i]->getHeight()) )
			{
				//printf("Best Eye: \n\tPos: %6.2f, %6.2f, \n\tSize: %6.2f, %6.2f, \n\tAngle: %6.2f\n", 
				//			  eyes[i].center.x, eyes[i].center.y, 
				//			  eyes[i].size.width, eyes[i].size.height,
				//			  eyes[i].angle);
				cvDrawRect(original_eyes[i],
									 cvPoint(eyes[i].x, eyes[i].y), 
									 cvPoint(eyes[i].x + eyes[i].width, eyes[i].y + eyes[i].height),
									 CV_RGB(0, 0, 255), 2);
			}
			//else
			//{
			//	printf("No eye detected.\n");
			//}
			cvShowImage(modified_names[i], original_eyes[i]);
			eye_trackers[i]->noPupilDetected = (pupils[i].radius <= MIN_PUPIL_RADIUS);
			eye_trackers[i]->pupilPositionX = pupils[i].x;
			eye_trackers[i]->pupilPositionY = pupils[i].y;
			eye_trackers[i]->eyeSizeX = eyes[i].width;
			eye_trackers[i]->eyeSizeY = eyes[i].height;
			eye_trackers[i]->eyePositionX = eyes[i].x;
			eye_trackers[i]->eyePositionY = eyes[i].y;
			eye_trackers[i]->browPositionY = eyebrows[i].center.y;
		}
		if (send_data)
			recog->updateState(*eye_trackers[0], *eye_trackers[1]);
	}

	// Free memory allocated for input captures
	for (i=0; i<NUM_INPUTS; i++)
	{
		cvReleaseImage(&original_eyes[i]);
		cvReleaseImage(&modified_eyes[i]);
		cvReleaseImage(&threshold_eyes[i]);
		cvDestroyWindow(modified_names[i]);
		cvDestroyWindow(eyebrow_names[i]);
		cvDestroyWindow(eye_names[i]);
		cvDestroyWindow(pupil_names[i]);
		delete captures[i];
		delete eye_trackers[i];
		captures[i] = NULL;
	}
	delete recog;

	delete canny_high;
	delete canny_low;
	delete pupil_threshold;
	delete eyebrow_threshold;
	delete eye_threshold;
	cvDestroyWindow(config_name);

	return 1;
}