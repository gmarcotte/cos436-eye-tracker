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


void calibration_mouse_handler(
		int event, int x, int y, int flags, void* param);

class Pupil {
public:
	int x;
	int y;
	int radius;
};

void draw_box(IplImage* img, CvRect rect, CvScalar color)
{
	cvRectangle(
		img,
		cvPoint(rect.x, rect.y),
		cvPoint(rect.x + rect.width, rect.y + rect.height),
		color, 2);
}

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


CvRect box;
bool drawing_box = false;
bool need_update = false;
bool drawn = false;

int main(int argc, char* argv[])
{
	int i;
	char c;
	bool from_camera = false;
	CaptureHandler* capture = NULL;
	if (1 < argc)
	{
		// The user supplied a command line path for video analysis
		try {
			capture = new CaptureHandler(argv[1]);
		} catch(CaptureHandler::FileReadError) {
			printf("Read Error: skipping file %s\n", argv[1]);
			delete capture;
		}
		printf("Tracking from %s:\n", argv[1]);
		capture->printFullState();
		printf("\n\n");
	}

	while (!capture)
	{
		// No command-line argument was given, so try to load a camera
		try {
			capture = new CaptureHandler();
		} catch(CaptureHandler::CameraReadError) {
			printf("ERROR: Please connect a camera, and then press enter.\n");
			getchar();
			continue;
		}
		printf("Tracking from camera:\n");
		from_camera = true;
		capture->printFullState();
	}

	char* original_name = "Direct Camera Feed";
	cvNamedWindow(original_name);
	IplImage* original_img = cvCreateImage(cvSize(capture->getWidth(), capture->getHeight()),
																				 IPL_DEPTH_8U, 3);

	printf("\n\n******** ENTERING CALIBRATION **************\n\n");
	printf("Please position the camera so that both eye regions (including eyebrows and forehead) are clearly visible.\n");
	printf("Press SPACEBAR to continue...\n");
	while( (c = cvWaitKey(33)) != 32)
	{
		capture->advance();
		capture->getCurrentFrame(original_img, CV_CVTIMG_FLIP);
		cvFlip(original_img, 0, 1);
		cvShowImage(original_name, original_img);
	}

	IplImage* shown_img = cvCloneImage(original_img);
	CvRect regions[NUM_INPUTS];

	cvSetMouseCallback(original_name, calibration_mouse_handler);
	printf("Please select the full eye regions, one at a time.  Be sure to include the full eyebrow and forehead.\n");
	printf("Press SPACEBAR after each selection...\n");
	box = cvRect(-1, -1, 0, 0);
	CvScalar colors[] = {CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(0, 0, 255),
										   CV_RGB(255, 255, 0), CV_RGB(255, 0, 255), CV_RGB(0, 255, 255)};
	for (i=0; i<NUM_INPUTS; i++)
	{
		printf("Please select REGION %d!\n", i);
		while ( ((c = cvWaitKey(15)) != 32) || !drawn)
		{
			cvCopyImage(original_img, shown_img);
			if (drawing_box || drawn)
				draw_box(shown_img, box, colors[i%6]);
			cvShowImage(original_name, shown_img);
		}
		regions[i] = box;
		draw_box(original_img, box, colors[i%6]);
		drawn = false;
		drawing_box = false;
	}
	
	printf("Thank you.  Please check the windows to ensure regions were chosen properly.\n\n");
	
	// Set up the eye trackers, image memory and display windows for each input
	IplImage* original_eyes[NUM_INPUTS];
	IplImage* modified_eyes[NUM_INPUTS];
	IplImage* threshold_eyes[NUM_INPUTS];

	char modified_names[NUM_INPUTS][MAX_STRING];
	char eyebrow_names[NUM_INPUTS][MAX_STRING];
	char pupil_names[NUM_INPUTS][MAX_STRING];
	for (i=0; i<NUM_INPUTS; i++)
	{
		sprintf_s(modified_names[i], MAX_STRING, "Input %d: Tracking Results", i);
		sprintf_s(eyebrow_names[i], MAX_STRING, "Input %d: Thresholded Eyebrow", i);
		sprintf_s(pupil_names[i], MAX_STRING, "Input %d: Thresholded Pupil", i);
		cvNamedWindow(modified_names[i]);
		cvNamedWindow(eyebrow_names[i]);
		cvNamedWindow(pupil_names[i]);
		
		original_eyes[i] = cvCreateImage(cvSize(regions[i].width, regions[i].height),
									    							 IPL_DEPTH_8U, 3);
		modified_eyes[i] = cvCreateImage(cvSize(regions[i].width, regions[i].height),
									    							 IPL_DEPTH_8U, 1);
	  threshold_eyes[i] = cvCreateImage(cvSize(regions[i].width, regions[i].height),
						 			    							  IPL_DEPTH_8U, 1);
	}

	// Set up the config GUI
	char* config_name = "EyeTracker Configuration";
	cvNamedWindow(config_name, 0);
	
	ConfigTrackbar* pupil_thresholds[NUM_INPUTS];
	ConfigTrackbar* eyebrow_thresholds[NUM_INPUTS];

	for (i=0; i<NUM_INPUTS; i++)
	{
		char pupil_name[MAX_STRING];
		sprintf_s(pupil_name, MAX_STRING, "P Thresh %d", i);
		pupil_thresholds[i] = new ConfigTrackbar(
			pupil_name, config_name, 20, 0, 255, 255);

		char eyebrow_name[MAX_STRING];
		sprintf_s(eyebrow_name, MAX_STRING, "EB Thresh %d", i);
		eyebrow_thresholds[i] = new ConfigTrackbar(
			eyebrow_name, config_name, 85, 0, 255, 255);
	}

	Recognizer* recog = new Recognizer();

	Eye* eye_trackers[NUM_INPUTS];
	Pupil pupils[NUM_INPUTS];
	CvBox2D eyebrows[NUM_INPUTS];
	for (i = 0; i < NUM_INPUTS; i++)
	{
		eye_trackers[i] = new Eye();

		pupils[i].x = 0;
		pupils[i].y = 0;
		pupils[i].radius = 0;

		eyebrows[i].angle = 0;
		eyebrows[i].center = cvPoint2D32f(0, 0);
		eyebrows[i].size = cvSize2D32f(0, 0);
	}
	
	
	bool calibrating = true;
	int calibration_stage = -1;	
	int calibration_timer = -1;
	bool send_data = false;
	printf("Please adjust the sliders at left until the pupil and eyebrow are stably tracked.\n");
	printf("Press SPACEBAR when you are ready to continue.\n\n");
	
	// Quit with Escape
	while ( (c = cvWaitKey(33)) != 27)
	{
		// Start and stop recognition with Spacebar
		if (c == 32 && !calibrating)
			send_data = !send_data;
		
		cvResetImageROI(original_img);
		capture->advance();
		capture->getCurrentFrame(original_img, CV_CVTIMG_FLIP);
		cvFlip(original_img, 0, 1);

		for (i = 0; i < NUM_INPUTS; i++)
		{
			cvSetImageROI(original_img, regions[i]);
			cvCopy(original_img, original_eyes[i]);
			cvZero(modified_eyes[i]);
			cvCvtPixToPlane(original_img, NULL, NULL, modified_eyes[i], NULL );
			cvSmooth(modified_eyes[i], modified_eyes[i], CV_GAUSSIAN, 5, 5);

			// Pupil detection
			cvThreshold(modified_eyes[i], threshold_eyes[i], pupil_thresholds[i]->getValue(), 255, CV_THRESH_BINARY_INV);
			cvCanny(threshold_eyes[i], threshold_eyes[i], CANNY_LOW, CANNY_HIGH, 3);
			cvShowImage(pupil_names[i], threshold_eyes[i]);
			updatePupil(threshold_eyes[i], &pupils[i]);
			//printf("Found pupil %d: (%d, %d) - %d\n", i, pupils[i].x, pupils[i].y, pupils[i].radius);
			if (pupils[i].radius > MIN_PUPIL_RADIUS)
				cvDrawCircle(original_eyes[i], cvPoint(pupils[i].x, pupils[i].y), pupils[i].radius, CV_RGB(255, 0, 0), 2);


			// Eyebrow Detection
			cvThreshold(modified_eyes[i], threshold_eyes[i], eyebrow_thresholds[i]->getValue(), 255, CV_THRESH_BINARY_INV);
			cvShowImage(eyebrow_names[i], threshold_eyes[i]);
			updateEyebrow(threshold_eyes[i], &eyebrows[i], &pupils[i]);		
			if ( (eyebrows[i].center.x >= 0 && eyebrows[i].center.x <= capture->getWidth()) &&
					 (eyebrows[i].center.y >= 0 && eyebrows[i].center.y <= capture->getHeight()) )
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
			cvShowImage(modified_names[i], original_eyes[i]);
			eye_trackers[i]->noPupilDetected = (pupils[i].radius <= MIN_PUPIL_RADIUS);
			eye_trackers[i]->pupilPositionX = pupils[i].x;
			eye_trackers[i]->pupilPositionY = pupils[i].y;
			eye_trackers[i]->pupilRadius = pupils[i].radius;
			eye_trackers[i]->browPositionY = eyebrows[i].center.y;
			eye_trackers[i]->browHeight = min(eyebrows[i].size.height, eyebrows[i].size.width);
		}
		if (send_data)
			recog->updateState(*eye_trackers[0], *eye_trackers[1]);
			
		if (calibrating)
		{
			if (calibration_stage < 0 && c == 32)
			{
				calibration_stage++;
				printf("Without moving your head, please look to the LEFT.\n");
				printf("Press SPACEBAR and continue to look LEFT for a few moments\n");
				printf("before looking back at the screen.\n\n");
			}
			else if (calibration_stage >= NUM_TO_CALIBRATE)
			{
				printf("Thank you for calibrating!\n");
				printf("********** END OF CALIBRATION ***************\n\n");
				printf("Please press SPACEBAR to begin recognition\n\n");
				calibrating = false;
				calibration_stage = -1;
				calibration_timer = -1;
			}
			else if (c == 32 && calibration_timer < 0)
			{
				calibration_timer++;
			}
			else if (calibration_timer >= 0)
			{
				recog->updateLeftCalibration(*eye_trackers[0], calibration_stage);
				recog->updateRightCalibration(*eye_trackers[1], calibration_stage);
				calibration_timer++;
			}
			
			if (calibration_timer >= CALIBRATION_FRAMES)
			{
				calibration_timer = -1;
				calibration_stage++;
				switch(calibration_stage)
				{
				case 1:
				{
					printf("Without moving your head, please look to the RIGHT.\n");
					printf("Press SPACEBAR and continue to look RIGHT for a few moments\n");
					printf("before looking back at the screen.\n\n");
				}
				break;
				case 2:
				{
					printf("Without moving your head, please look UP.\n");
					printf("Press SPACEBAR and continue to look UP for a few moments\n");
					printf("before looking back at the screen.\n\n");
				}
				break;
				case 3:
				{
					printf("Without moving your head, please look DOWN.\n");
					printf("Press SPACEBAR and continue to look DOWN for a few moments\n");
					printf("before looking back at the screen.\n\n");
				}
				break;
				case 4:
				{
					printf("Without moving your head, please RAISE YOUR EYEBROWS.");
					printf("Press SPACEBAR and continue RAISING YOUR EYEBROWS for a\n");
					printf("few moments before returning them to a rest position.\n\n");
				}
				break;
				}
			}
		}
	}

	// Free memory allocated for input captures
	for (i=0; i<NUM_INPUTS; i++)
	{
		cvReleaseImage(&modified_eyes[i]);
		cvReleaseImage(&threshold_eyes[i]);
		cvDestroyWindow(modified_names[i]);
		cvDestroyWindow(eyebrow_names[i]);
		cvDestroyWindow(pupil_names[i]);
		delete eye_trackers[i];
		delete pupil_thresholds[i];
		delete eyebrow_thresholds[i];
	}
	delete recog;
	delete capture;
	cvDestroyWindow(config_name);

	return 1;
}



void calibration_mouse_handler(int event, int x, int y, int flas, void* param)
{
	switch(event)
	{
	case CV_EVENT_MOUSEMOVE:
	{
		if(drawing_box)
		{
			box.width = x - box.x;
			box.height = y - box.y;
		}
	}
	break;
	case CV_EVENT_LBUTTONDOWN:
	{
		drawing_box = true;
		drawn = false;
		box = cvRect(x, y, 0, 0);
	}
	break;
	case CV_EVENT_LBUTTONUP:
	{
		drawing_box = false;
		drawn = true;
		if (box.width < 0) {
			box.x += box.width;
			box.width *= -1;
		}
		if (box.height < 0) {
			box.y += box.height;
			box.height *= -1;
		}
	}
	break;
	}
}