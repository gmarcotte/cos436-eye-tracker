#include <stdio.h>

#include <cv.h>
#include <highgui.h>

#include "CaptureHandler.h"
#include "config.h"

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
	cvEyeTracker* trackers[NUM_INPUTS];
	IplImage* original_eyes[NUM_INPUTS];
	IplImage* modified_eyes[NUM_INPUTS];
	IplImage* ellipses[NUM_INPUTS];
	IplImage* thresholds[NUM_INPUTS];

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

		trackers[i] = new cvEyeTracker(captures[i]->getWidth(), captures[i]->getHeight());
		original_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																		 IPL_DEPTH_8U, 1);
		modified_eyes[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																		 IPL_DEPTH_8U, 1);
		ellipses[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																IPL_DEPTH_8U, 3);
		thresholds[i] = cvCreateImage(cvSize(captures[i]->getWidth(), captures[i]->getHeight()),
																	IPL_DEPTH_8U, 3);
	}

	char c;
	while ( (c = cvWaitKey(0)) != 'q')
	{
		for (i = 0; i < NUM_INPUTS; i++)
		{
			captures[i]->advance();
			captures[i]->getCurrentFrame(original_eyes[i]);
			trackers[i]->process(original_eyes[i]);
			trackers[i]->getEyeImage(modified_eyes[i]);
			trackers[i]->getEllipseImage(ellipses[i]);
			trackers[i]->getThresholdImage(thresholds[i]);
			cvShowImage(modified_names[i], modified_eyes[i]);
			cvShowImage(ellipse_names[i], ellipses[i]);
			cvShowImage(threshold_names[i], thresholds[i]);
		}
	}


	// Free memory allocated for input captures
	for (i=0; i<NUM_INPUTS; i++)
	{
		cvReleaseImage(&original_eyes[i]);
		cvReleaseImage(&modified_eyes[i]);
		cvReleaseImage(&ellipses[i]);
		cvReleaseImage(&thresholds[i]);
		cvDestroyWindow(modified_names[i]);
		cvDestroyWindow(ellipse_names[i]);
		cvDestroyWindow(threshold_names[i]);
		delete trackers[i];
		trackers[i] = NULL;
		delete captures[i];
		captures[i] = NULL;
	}

	return 1;
}