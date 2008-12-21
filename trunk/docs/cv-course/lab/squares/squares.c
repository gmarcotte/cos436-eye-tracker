//
// The full "Square Detector" program.
// It loads several images subsequentally and tries to find squares in
// each image
//

#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

int thresh = 50;
IplImage* img = 0;
IplImage* img0 = 0;
CvMemStorage* storage = 0;
CvPoint pt[4];

#define dot(a,b) (a.x*b.x + a.y*b.y)

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2 
double angle( CvPoint pt1, CvPoint pt2, CvPoint pt0 )
{
    double a;
    pt1.x -= pt0.x;
    pt2.x -= pt0.x;
    pt1.y -= pt0.y;
    pt2.y -= pt0.y;
    a = dot(pt1,pt2)/sqrt((double)dot(pt1,pt1)*dot(pt2,pt2) + 1e-10);
    return a;
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
CvSeq* findSquares4( IplImage* img, CvMemStorage* storage )
{
    CvSeq* contours;
    int i, c, l, N = 11;
    CvSize sz = cvSize( img->width & -2, img->height & -2 );
    IplImage* timg = cvCloneImage( img ); // make a copy of input image
    IplImage* gray = cvCreateImage( sz, 8, 1 ); 
    IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 3 );
    IplImage* tgray;
    // create empty sequence that will contain points -
    // 4 points per square (the square's vertices)
    CvSeq* squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );
    
    // select the maximum ROI in the image
    // with the width and height divisible by 2
    cvSetImageROI( timg, cvRect( 0, 0, sz.width, sz.height ));
    
    // down-scale and upscale the image to filter out the noise
    cvPyrDown( timg, pyr, 7 );
    cvPyrUp( pyr, timg, 7 );
    tgray = cvCreateImage( sz, 8, 1 );
    
    // find squares in every color plane of the image
    for( c = 0; c < 3; c++ )
    {
        // extract the c-th color plane
        cvSetImageCOI( timg, c+1 );
        cvCopy( timg, tgray, 0 );
        
        // try several threshold levels
        for( l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading 	
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging) 
                cvCanny( tgray, gray, 0, thresh, 5 );
                // dilate canny output to remove potential
                // holes between edge segments 
                cvDilate( gray, gray, 0, 1 );
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                cvThreshold( tgray, gray, (l+1)*255/N, 255, CV_THRESH_BINARY );
            }
            
            // find contours and store them all as a list
            cvFindContours( gray, storage, &contours, sizeof(CvContour),
                CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );
            
            // test each contour
            while( contours )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                CvSeq* result = cvApproxPoly( contours, sizeof(CvContour), storage,
                    CV_POLY_APPROX_DP, cvContourPerimeter(contours,CV_WHOLE_SEQ)*0.02, 0 );
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( result->total == 4 &&
                    fabs(cvContourArea(result,CV_WHOLE_SEQ)) > 1000 &&
                    cvCheckContourConvexity(result) )
                {
                    double s = 0, t;
                    
                    for( i = 0; i < 5; i++ )
                    {
                        // store all the contour vertices in the buffer
                        pt[i&3] = *(CvPoint*)cvGetSeqElem( result, i, 0 );
                        // and find minimum angle between joint
                        // edges (maximum of cosine)
                        if( i >= 2 )
                        {
                            t = fabs(angle( pt[i&3], pt[(i-2)&3], pt[(i-1)&3]));
                            s = MAX( s, t );
                        }
                    }
                    
                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence 
                    if( s < 0.3 )
                        for( i = 0; i < 4; i++ )
                            cvSeqPush( squares, pt + i );
                }
                
                // take the next contour
                contours = contours->h_next;
            }
        }
    }
    
    // release all the temporary images
    cvReleaseImage( &gray );
    cvReleaseImage( &pyr );
    cvReleaseImage( &tgray );
    cvReleaseImage( &timg );
    
    return squares;
}


// the function draws all the squares in the image
void drawSquares( IplImage* img, CvSeq* squares )
{
    CvSeqReader reader;
    IplImage* cpy = cvCloneImage( img );
    int i;
    
    // initialize reader of the sequence
    cvStartReadSeq( squares, &reader, 0 );
    
    // read 4 sequence elements at a time (all vertices of a square)
    for( i = 0; i < squares->total; i += 4 )
    {
        CvPoint* rect = pt;
        int count = 4;
        
        // read 4 vertices
        CV_READ_SEQ_ELEM( pt[0], reader );
        CV_READ_SEQ_ELEM( pt[1], reader );
        CV_READ_SEQ_ELEM( pt[2], reader );
        CV_READ_SEQ_ELEM( pt[3], reader );
        
        // draw the square as a closed polyline 
        cvPolyLine( cpy, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, 8 );
    }
    
    // show the resultant image
    cvvShowImage("image",cpy);
    cvReleaseImage( &cpy );
}


void on_trackbar( int a )
{
    if( img )
        drawSquares( img, findSquares4( img, storage ) );
}

char* names[] = { "../pic1.bmp", "../pic2.bmp", "../pic3.bmp",
                  "../pic5.bmp", "../pic6.bmp", "../pic7.bmp" };

int main()
{
    int i;
    // create memory storage that will contain all the dynamic data
    storage = cvCreateMemStorage(0);
    // create window with name "image"
    cvvNamedWindow( "image", 1 );
    // create trackbar (slider) with parent "image" and set callback
    // (the slider regulates upper threshold, passed to Canny edge detector) 
    cvvCreateTrackbar( "thresh1", "image", &thresh, 1000, on_trackbar );
    
    for( i = 0; i < 6; i++ )
    {
        // load i-th image
        img0 = cvvLoadImage( names[i] );
        img = cvCloneImage( img0 );
        // force the image processing
        on_trackbar(0);
        // wait for key.
        // Also the function cvvWaitKey takes care of event processing
        cvvWaitKey(0);
        // release both images
        cvReleaseImage( &img );
        cvReleaseImage( &img0 );
        // clear memory storage - reset free space position
        cvClearMemStorage( storage );
    }
    
    return 0;
}

