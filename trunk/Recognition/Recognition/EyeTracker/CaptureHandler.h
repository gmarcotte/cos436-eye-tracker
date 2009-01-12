// A class for wrapping useful functionality of the CvCapture object.

#ifndef CAPTUREHANDLER_CLASS
#define CAPTUREHANDLER_CLASS

#include <cv.h>
#include <highgui.h>

class CaptureHandler
{
private:
  IplImage* frame;
  CvCapture* capture;
  CvSize size;
  double framerate;
  double num_frames;
  int position;
  char* window_name;

public:
  // Constructors and Destructors
  CaptureHandler();       // Default camera
	CaptureHandler(int);		// Camera specified by number
  CaptureHandler(char*);  // Video
  ~CaptureHandler();

  // Basic accessors for member data
  int getHeight() const;
  int getWidth() const;
  double getFrameRate() const;
  double getNumFrames() const;
  int getPosition() const;

  // More complex accessors
  void getCurrentFrame(IplImage*, int) const;
  void refreshProperties();

  // Methods for printing state information
  void printFullState() const;
  void printStatus() const;

  // Actions for playing the video
  void openInWindow(char*);
  void play(double rate = 0);
  void advance();
  void seek(int);
  void closeWindow();

  // Actions for saving video data
  void write(char*, int);

  // Exceptions
  class FileReadError {};
  class CameraReadError {};
  class FrameError {};
  class WindowError {};
};


#endif