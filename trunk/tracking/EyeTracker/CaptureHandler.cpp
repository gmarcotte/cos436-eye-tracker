// Implementation of the CaptureHandler class

#include <stdio.h>
#include "CaptureHandler.h"
#include <cv.h>
#include <highgui.h>

#define DEFAULT_FRAMERATE 30


// Constructor: Connect to the primary camera
CaptureHandler::CaptureHandler()
{
	this->capture = cvCaptureFromCAM(0);
  this->position = 0;
  this->window_name = NULL;
  if(!this->capture)
    throw CameraReadError();

  this->frame = cvQueryFrame(this->capture);
  if(!this->frame)
    throw FrameError();
  this->refreshProperties();
}

// Constructor: Connect to a specified camera
CaptureHandler::CaptureHandler(int num)
{
	this->capture = cvCaptureFromCAM(num);
  this->position = 0;
  this->window_name = NULL;
  if(!this->capture)
    throw CameraReadError();

  this->frame = cvQueryFrame(this->capture);
  if(!this->frame)
    throw FrameError();
  this->refreshProperties();
}

// Constructor: Load a video from a file
CaptureHandler::CaptureHandler(char *filename)
{
  this->capture = cvCreateFileCapture(filename);
  this->position = 0;
  this->window_name = NULL;
  if(!this->capture)
    throw FileReadError();

  this->frame = cvQueryFrame(this->capture);
  if(!this->frame)
    throw FrameError();
  this->refreshProperties();
}

// Destructor: Release memory used by the capture
CaptureHandler::~CaptureHandler()
{
  if(this->capture)
    cvReleaseCapture(&this->capture);
  if(this->window_name)
    cvDestroyWindow(this->window_name);
}

// Basic accessors
int CaptureHandler::getHeight() const {return this->size.height;}
int CaptureHandler::getWidth() const {return this->size.width;}
double CaptureHandler::getNumFrames() const {return this->num_frames;}
int CaptureHandler::getPosition() const {return this->position;}
double CaptureHandler::getFrameRate() const {return (this->framerate) ? this->framerate : DEFAULT_FRAMERATE;}

// Get the current frame as a separately allocated image object.
void CaptureHandler::getCurrentFrame(IplImage* fr) const
{
  cvConvertImage(this->frame, fr);
}

// Update the properties by querying the capture object
void CaptureHandler::refreshProperties()
{
  this->size.height = (int)cvGetCaptureProperty(this->capture, CV_CAP_PROP_FRAME_HEIGHT);
  this->size.width = (int)cvGetCaptureProperty(this->capture, CV_CAP_PROP_FRAME_WIDTH);
  this->framerate = cvGetCaptureProperty(this->capture, CV_CAP_PROP_FPS);
  this->num_frames = cvGetCaptureProperty(this->capture, CV_CAP_PROP_FRAME_COUNT);
}

// Open a new window and show the current frame of the video
void CaptureHandler::openInWindow(char *window_name)
{
  this->window_name = window_name;
  cvNamedWindow(this->window_name);
  cvShowImage(this->window_name, this->frame);
}

// Play the video in the opened window.  Stops on escape.
void CaptureHandler::play(double rate)
{
  if(!this->window_name)
    throw WindowError();

  if(!rate)
    rate = this->getFrameRate();

  int delay = (int) (1000 / rate);

  while(1)
  {
    try
    {
      this->advance();
    } 
    catch(FrameError)
    {
      break;
    }
    if(cvWaitKey(delay) == 27)
      break;
  }
}

// Move forward one frame, updating the display window if possible.
void CaptureHandler::advance()
{
  this->frame = cvQueryFrame(this->capture);
  (this->position)++;
  if(!this->frame)
    throw FrameError();
  if(this->window_name)
    cvShowImage(this->window_name, this->frame);
}

// Move to the selected frame and update the display window if possible.
void CaptureHandler::seek(int pos)
{
  cvSetCaptureProperty(this->capture, CV_CAP_PROP_POS_FRAMES,	pos);
  this->position = pos;
  this->frame = cvQueryFrame(capture);
  if(!frame)
    throw FrameError();
  if(this->window_name)
    cvShowImage(this->window_name, this->frame);
}

// Release the memory for the open window, if there is one.
void CaptureHandler::closeWindow()
{
  if(this->window_name)
  {
    cvDestroyWindow(this->window_name);
    this->window_name = NULL;
  }
}


// Save part of the video to disk
void CaptureHandler::write(char* filename, int frames)
{
  CvVideoWriter* video_writer = cvCreateVideoWriter(filename, -1, this->getFrameRate(), cvGetSize(this->frame) );
  printf("Starting write in 3...");
  Sleep(1000);
  printf("2...");
  Sleep(1000);
  printf("1...");
  Sleep(1000);
  printf("NOW!");
  int i;
  for (i=0; i < frames; i++)
  {
    cvWriteFrame(video_writer, this->frame);
    try
    {
      this->advance();
    }
    catch (FrameError)
    {
      break;
    }
  }
  cvReleaseVideoWriter(&video_writer);
}

// Print out all variables.
void CaptureHandler::printFullState() const
{
  printf("Video Height: %d\n", this->size.height);
  printf("Video Width: %d\n", this->size.width);
  printf("Frame Rate: %d\n", this->framerate);
  printf("Total Frames: %d\n", this->num_frames);
  printf("Current Position: %d\n", this->position);
  if(this->window_name)
    printf("Playing in Window: %s\n", window_name);
}

// Print a one line summary.
void CaptureHandler::printStatus() const
{
  printf("At frame %d", this->position);
  if(this->window_name)
    printf(" in window %s.\n", this->window_name);
  else
    printf(".\n");
}

  



