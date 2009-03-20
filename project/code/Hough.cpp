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
      CV_RGB(0,256,0));
 
  } 

  const char * window = "cvHoughCircles";
   if(results->total>0)
  {
	feature_values.push_back(results->total);	
  	//cvNamedWindow( window, 1 ); 
  	//cvShowImage( window, image);
  } 
  
  else
  {
	feature_values.push_back(0);
  }

/*   CvHistogram * hist;
  int bins = 64;
  int hist_size[] = {bins};
  hist = cvCreateHist(1,hist_size, CV_HIST_ARRAY, NULL, 1);
  cvCalcHist(&image,hist);

  for(int i =0; i<bins; i++)
  {
        feature_values.push_back(cvQueryHistValue_1D(hist,i));
  }*/

 
cvReleaseMemStorage(&storage); 
}


/**
 *  * Returns Hough Circles
 *   * Precondition: image should be 64x64
 *    */
void Hough::getCorners(vector<double>& feature_values,
                                    const IplImage* img) const {
  assert(img->width == 64);
  assert(img->height == 64);

  IplImage* image = ((IplImage*)(img));

   int MAX_CORNERS = 500; 


   IplImage* eig_image = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 ); 
  IplImage* tmp_image = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 ); 
  int           corner_count = MAX_CORNERS; 
  CvPoint2D32f* cornersA     = new CvPoint2D32f[ MAX_CORNERS ]; 
  cvGoodFeaturesToTrack( 
    image, 
    eig_image, 
    tmp_image, 
    cornersA, 
    &corner_count, 
    0.01, 
    5.0, 
    0, 
    3, 
    0, 
    0.04 
  ); 


   cvReleaseImage(&eig_image);
   cvReleaseImage(&tmp_image);
    feature_values.push_back(corner_count);
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
      //cvNamedWindow( window, 1 );
      //cvShowImage( window, image);
  }

   CvHistogram * hist;
  int bins = 2;
  int hist_size[] = {bins};
  hist = cvCreateHist(1,hist_size, CV_HIST_ARRAY, NULL, 1);
  cvCalcHist(&image,hist);
  
  for(int i =0; i<bins; i++)
  {
	feature_values.push_back(cvQueryHistValue_1D(hist,i));
  }
 
  cvReleaseMemStorage(&storage);
  cvReleaseHist(&hist);
}


  
