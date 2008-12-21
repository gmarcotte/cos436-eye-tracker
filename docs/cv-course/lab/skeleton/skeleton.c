//
// The simplest OpenCV program.
// It creates window and displays a sequence of images in it 
//

#include "cv.h"  // include core library interface
#include "highgui.h" // include GUI library interface
#include <stdio.h>
#include <math.h>
#include <string.h>

IplImage* img = 0; // Declare IPL/OpenCV image pointers 
IplImage* img0 = 0;

char* names[] = { "../pic1.bmp", "../pic2.bmp", "../pic3.bmp",
                  "../pic5.bmp", "../pic6.bmp", "../pic7.bmp" };

int main()
{
    int i;
    cvvNamedWindow( "image", 1 ); // create HighGUI window with name "image"
    
    for( i = 0; i < 6; i++ )
    {
        img0 = cvvLoadImage( names[i] ); // load i-th image
        img = cvCloneImage( img0 ); // make a copy of it
        cvvShowImage( "image", img ); // show image in the window "image"
        cvvWaitKey(0); // wait for key. The function has
                       // an event processing loop inside
        cvReleaseImage( &img ); // release both images
        cvReleaseImage( &img0 );
    }
    
    return 0;
}
