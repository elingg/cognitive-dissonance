#include "Hough.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <math.h> 

//TODO: Use C++ classes properly


Hough::Hough() {
}

Hough::~Hough() {}

/**
 * Returns Hough Circles 
 * Precondition: image should be 64x64
 */
void Hough::getCircles(vector<double>& feature_values,
                                    const IplImage* img) const {
  assert(img->width == 64);
  assert(img->height == 64);
  
  IplImage* image = ((IplImage*)(img));
 
 CvMemStorage* storage = cvCreateMemStorage(0); 
  cvSmooth(image, image, CV_GAUSSIAN, 5, 5 ); 

 CvSeq* results = cvHoughCircles( 
    image, 
    storage, 
    CV_HOUGH_GRADIENT, 
    2, 
    image->width/10 
  ); 

  for( int i = 0; i < results->total; i++ ) { 
    float* p = (float*) cvGetSeqElem( results, i ); 
    CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) ); 
     cvCircle( 
      ((CvArr*)image), 
      pt, 
      cvRound( p[2] ), 
      CV_RGB(0xff,0xff,0xff));
 
  } 

  const char * window = "cvHoughCircles";
   if(results->total>0)
  {
  	cvNamedWindow( window, 1 ); 
  	cvShowImage( window, image);
  } 

cvReleaseMemStorage(&storage); 
}


/**
 *  * Returns Hough Lines 
 *   * Precondition: image should be 64x64
 *    */
void Hough::getEdges(vector<double>& feature_values,
                                    const IplImage* img) const {
  assert(img->width == 64);
  assert(img->height == 64);

  IplImage* image = ((IplImage*)(img));


  CvMemStorage* storage = cvCreateMemStorage(0);
cvThreshold(image, image, 160, 255, CV_THRESH_BINARY);
cvCanny(image, image, 0.5, 0.5, 3); 
cvThreshold(image,image,160,255, CV_THRESH_BINARY_INV);
/* CvSeq* lines = cvHoughLines2(
    image,
    storage,
    CV_HOUGH_STANDARD,
    1,
   CV_PI/180, 
   120 
    
  );*/




/*    for( int i = 0; i < MIN(lines->total,100); i++ )
        {
            float* line = (float*)cvGetSeqElem(lines,i);
            float rho = line[0];
            float theta = line[1];
            CvPoint pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            cvLine( (CvArr*)image, pt1, pt2, CV_RGB(0,255,0), 3, 8 );
        }*/

  const char * window = "cvHoughLines";
  //if(lines->total>0)
  {
      cvNamedWindow( window, 1 );
      cvShowImage( window, image);
  }

  cvReleaseMemStorage(&storage);
}

