
#include <stdio.h>
#include <time.h>

#include "CaptureHandler.h"
#include <cv.h>
#include <highgui.h>


#define NUM_FRAMES 500

void sleep(int seconds)
{
  clock_t endwait;
  endwait = clock () + seconds * CLOCKS_PER_SEC ;
  while (clock() < endwait) {}
}

int main(int argc, char* argv[])
{

	if (argc != 2)
	{
		printf("Must have exactly 1 command line argument.\n");
		return -1;
	}
  CaptureHandler *ch;
	printf("Connecting to camera...");
  try {
    ch = new CaptureHandler();
  }
  catch (CaptureHandler::CameraReadError)
  {
    printf("FAILED!\n");
    return -1;
  }
  printf("SUCCESS!\n");
	ch->printFullState();

	printf("Opening %s for writing...", argv[1]);
	CvVideoWriter *writer = cvCreateVideoWriter(argv[1], 
																							0,
																							30,
																							cvSize(ch->getWidth(), ch->getHeight()));
	if (!writer)
	{
		printf("FAILED!\n");
		delete ch;
		return -1;
	}
	printf("SUCCESS!\n");

  ch->openInWindow("Camera Feed");
	int i;
	char c;
	while ( (c = cvWaitKey(33)) != 'r')
		ch->advance();

	IplImage *frame;
	printf("Recording in 3...");
	sleep(1);
	printf("2...");
	sleep(1);
	printf("1...");
	sleep(1);
	printf("GO!\n");
	for (i=0; i<NUM_FRAMES; i++)
	{
		ch->advance();
		frame = ch->getCurrentFrame();
		cvWriteFrame(writer, frame);
		cvReleaseImage(&frame);
  }
	cvReleaseVideoWriter(&writer);
	delete ch;

  
	return 0;
};
