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

	char original_names[NUM_INPUTS][MAX_STRING];
	char modified_names[NUM_INPUTS][MAX_STRING];
	char ellipse_names[NUM_INPUTS][MAX_STRING];
	char threshold_names[NUM_INPUTS][MAX_STRING];
	for (i=0; i<NUM_INPUTS; i++)
	{
		sprintf_s(original_names[i], MAX_STRING, "Input %d: Direct Feed", i);
		sprintf_s(modified_names[i], MAX_STRING, "Input %d: Modified Eye Image", i);
		sprintf_s(ellipse_names[i], MAX_STRING, "Input %d: Ellipse Image", i);
		sprintf_s(threshold_names[i], MAX_STRING, "Input %d: Threshold Image", i);
		captures[i]->openInWindow(original_names[i]);
		cvNamedWindow(modified_names[i]);
		cvNamedWindow(ellipse_names[i]);
		cvNamedWindow(threshold_names[i]);

		original_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																		 IPL_DEPTH_8U, 3);
		modified_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																		 IPL_DEPTH_8U, 1);
	}

	// Set up the config GUI
	char* config_name = "EyeTracker Configuration";
	cvNamedWindow(config_name, 0);

	ConfigTrackbar* threshold = new ConfigTrackbar(
		"Threshold", config_name, 80, 0, 255, 255);

	ConfigTrackbar* canny_high = new ConfigTrackbar(
		"Canny Hi", config_name, 250, 0, 500, 500);

	ConfigTrackbar* canny_low = new ConfigTrackbar(
		"Canny Low", config_name, 200, 0, 500, 500);


	Pupil pupils[NUM_INPUTS];
	for (i = 0; i < NUM_INPUTS; i++)
	{
		pupils[i].x = 0;
		pupils[i].y = 0;
		pupils[i].radius = 0;
	}

	char c;
	while ( (c = cvWaitKey(33)) != 'q')
	{
		for (i = 0; i < NUM_INPUTS; i++)
		{
			captures[i]->advance();
			captures[i]->getCurrentFrame(original_eyes[i]);
			cvCvtPixToPlane(original_eyes[i], NULL, NULL, modified_eyes[i], NULL );
			cvThreshold(modified_eyes[i], modified_eyes[i], threshold->getValue(), 255, CV_THRESH_BINARY_INV);
			cvCanny(modified_eyes[i], modified_eyes[i], canny_low->getValue(), canny_high->getValue(), 3);
			cvShowImage(threshold_names[i], modified_eyes[i]);
			updatePupil(modified_eyes[i], &pupils[i]);
			printf("Found pupil %d: (%d, %d) - %d\n", i, pupils[i].x, pupils[i].y, pupils[i].radius);
			if (pupils[i].radius > 0)
				cvDrawCircle(original_eyes[i], cvPoint(pupils[i].x, pupils[i].y), pupils[i].radius, CV_RGB(255, 0, 0));
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
		cvDestroyWindow(modified_names[i]);
		cvDestroyWindow(ellipse_names[i]);
		cvDestroyWindow(threshold_names[i]);
		delete captures[i];
		captures[i] = NULL;
	}

	delete canny_high;
	delete canny_low;
	delete threshold;
	cvDestroyWindow(config_name);

	return 1;
}