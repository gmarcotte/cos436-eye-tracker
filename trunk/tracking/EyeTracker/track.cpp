#include <stdio.h>

#include <cv.h>
#include <highgui.h>

#include "CaptureHandler.h"
#include "config.h"
#include "GUI.h"

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
		if ( pupil->radius == 0 ||
				 (abs(box.center.x - pupil->x) < 100 &&
				  (pupil->y - box.center.y) > 10) )
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




void updateEye(IplImage* img, CvBox2D* eye, Pupil* pupil, CvBox2D* eyebrow)
{
	CvSeq* contours;
	IplImage* scratch = cvCloneImage(img);
	CvMemStorage* storage = cvCreateMemStorage(0);
	cvFindContours(scratch, storage, &contours);
	CvSeq* curr_contour = contours;

	int run_once = 0;
	CvBox2D best_eye;
	best_eye.center.x = -1;
	curr_contour = contours;
	while (curr_contour != NULL)
	{
		CvBox2D box = cvMinAreaRect2(curr_contour);
		if ( ((int)box.angle % 90) < 20 &&
				  (eyebrow->center.x != -1 ||
					 ( abs(box.center.x - eyebrow->center.x) < 100 &&
					   box.center.y - eyebrow->center.y > 0) ) &&
			    (pupil->radius == 0 ||
				   ( abs(box.center.x - pupil->x) < max(box.size.width, box.size.height) &&
					   abs(box.center.y - pupil->y) < max(box.size.width, box.size.height) ) ) )
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
	*(eye) = best_eye;
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


	Pupil pupils[NUM_INPUTS];
	CvBox2D eyebrows[NUM_INPUTS];
	CvBox2D eyes[NUM_INPUTS];
	for (i = 0; i < NUM_INPUTS; i++)
	{
		pupils[i].x = 0;
		pupils[i].y = 0;
		pupils[i].radius = 0;

		eyebrows[i].angle = 0;
		eyebrows[i].center = cvPoint2D32f(0, 0);
		eyebrows[i].size = cvSize2D32f(0, 0);

		eyes[i].angle = 0;
		eyes[i].center = cvPoint2D32f(0, 0);
		eyes[i].size = cvSize2D32f(0, 0);
	}

	char c;
	while ( (c = cvWaitKey(33)) != 'q')
	{
		for (i = 0; i < NUM_INPUTS; i++)
		{
			captures[i]->advance();
			captures[i]->getCurrentFrame(original_eyes[i]);
			cvZero(modified_eyes[i]);
			cvCvtPixToPlane(original_eyes[i], NULL, NULL, modified_eyes[i], NULL );
			cvSmooth(modified_eyes[i], modified_eyes[i], CV_GAUSSIAN, 5, 5);

			// Pupil detection
			cvThreshold(modified_eyes[i], threshold_eyes[i], pupil_threshold->getValue(), 255, CV_THRESH_BINARY_INV);
			cvCanny(threshold_eyes[i], threshold_eyes[i], canny_low->getValue(), canny_high->getValue(), 3);
			cvShowImage(pupil_names[i], threshold_eyes[i]);
			updatePupil(threshold_eyes[i], &pupils[i]);
			printf("Found pupil %d: (%d, %d) - %d\n", i, pupils[i].x, pupils[i].y, pupils[i].radius);
			if (pupils[i].radius > 0)
				cvDrawCircle(original_eyes[i], cvPoint(pupils[i].x, pupils[i].y), pupils[i].radius, CV_RGB(255, 0, 0), 2);


			// Eyebrow Detection
			cvThreshold(modified_eyes[i], threshold_eyes[i], eyebrow_threshold->getValue(), 255, CV_THRESH_BINARY_INV);
			cvShowImage(eyebrow_names[i], threshold_eyes[i]);
			updateEyebrow(threshold_eyes[i], &eyebrows[i], &pupils[i]);		
			if ( (eyebrows[i].center.x >= 0 && eyebrows[i].center.x <= captures[i]->getWidth()) &&
					 (eyebrows[i].center.y >= 0 && eyebrows[i].center.y <= captures[i]->getHeight()) )
			{
				printf("Found Eyebrow: \n\tPos: %6.2f, %6.2f, \n\tSize: %6.2f, %6.2f, \n\tAngle: %6.2f\n", 
							  eyebrows[i].center.x, eyebrows[i].center.y, 
							  eyebrows[i].size.width, eyebrows[i].size.height,
							  eyebrows[i].angle);
				cvDrawEllipse(original_eyes[i], 
											cvPoint(eyebrows[i].center.x, eyebrows[i].center.y), 
											cvSize(eyebrows[i].size.height / 2, eyebrows[i].size.width / 2), 
											eyebrows[i].angle, 0.0, 360.0, CV_RGB(0, 255, 0), 2);
			}
			else
			{
				printf("No eyebrow detected.\n");
			}
			

			// Eye Detection
			cvThreshold(modified_eyes[i], threshold_eyes[i], eye_threshold->getValue(), 255, CV_THRESH_BINARY_INV);
			cvShowImage(eye_names[i], threshold_eyes[i]);
			updateEye(threshold_eyes[i], &eyes[i], &pupils[i], &eyebrows[i]);	
			if ( (eyes[i].center.x >= 0 && eyes[i].center.x <= captures[i]->getWidth()) &&
					 (eyes[i].center.y >= 0 && eyes[i].center.y <= captures[i]->getHeight()) )
			{
				printf("Best Eye: \n\tPos: %6.2f, %6.2f, \n\tSize: %6.2f, %6.2f, \n\tAngle: %6.2f\n", 
							  eyes[i].center.x, eyes[i].center.y, 
							  eyes[i].size.width, eyes[i].size.height,
							  eyes[i].angle);
				cvDrawEllipse(original_eyes[i],
											cvPoint(eyes[i].center.x, eyes[i].center.y), 
											cvSize(eyes[i].size.height / 2, eyes[i].size.width / 2), 
											eyes[i].angle, 0.0, 360.0, CV_RGB(0, 0, 255), 2);
			}
			else
			{
				printf("No eye detected.\n");
			}

			cvShowImage(modified_names[i], original_eyes[i]);

			/* DO SOMETHING WITH pupils[i] HERE */



			/************************************/
		}
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
		captures[i] = NULL;
	}

	delete canny_high;
	delete canny_low;
	delete pupil_threshold;
	delete eyebrow_threshold;
	delete eye_threshold;
	cvDestroyWindow(config_name);

	return 1;
}