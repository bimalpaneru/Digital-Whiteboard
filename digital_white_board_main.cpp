#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#define loop while
const unsigned char deviceName = 1;
unsigned char pointCounter = 0;
unsigned char frameCounter = 0;
using namespace std;
using namespace cv;
#include <time.h>


class ThresholdTheImage
{
public:	

	//Constructor for initializing the range of HSV
	ThresholdTheImage()
	{
		//HSV thresholding parameters initialization 
		hLower = 0;
		sLower = 0;
		vLower = 172;
	    hUpper = 0;
	    sUpper = 134;
		vUpper = 255;

		//Gaussian Parameters initialization
		xKSize = 16;
		yKSize = 16;
		sigmaX = 3;
		sigmaY = 3;


	}

	//function to threshold
	Mat  thresholdRedimage( Mat &image )		
	{
		//creating a binary matrix 
		Mat imageThresholded = Mat( image.size(), CV_8UC1 );	
		
		//Gaussian Blur for filtering
		GaussianBlur( image, imageHSV, Size( 9, 9 ), 2.5, 2.5 );

		//Converting image to HSV
		cvtColor( image, imageHSV, CV_BGR2HSV );
				
		//Ranging to insert only the pixels in the range defined the below function
		inRange( imageHSV, Scalar( hLower, sLower, vLower ), Scalar( hUpper, sUpper, vUpper ),  imageThresholded );

		//Gaussina Blur to Blend the Deinterlace
		GaussianBlur( imageThresholded, imageThresholded, Size( 2*xKSize - 1, 2*yKSize - 1 ), ( sigmaX ), ( sigmaY ) );
				
		return imageThresholded;

	}	//end of the function thresholdRedimage

	//Funtion for creating trackbars (HSV ranging)
	void trackBarsForHSVLimits()
	{
		//Window for trackbar of HSV range
		namedWindow( "HSV Range" );

		//Creating trackbar to tune the laser pointer threshold
		createTrackbar( "H-lower", "HSV Range", &hLower, 255, 0 );
		createTrackbar( "H-upper", "HSV Range", &hUpper, 255, 0 );
		createTrackbar( "S-lower", "HSV Range", &sLower, 255, 0 );
		createTrackbar( "S-upper", "HSV Range", &sUpper, 255, 0 );
		createTrackbar( "V-lower", "HSV Range", &vLower, 255, 0 );
		createTrackbar( "V-upper", "HSV Range", &vUpper, 255, 0 );

	}	// end of function trackBarsForHSVLimits

	//Function for Gaussian blur of thresholded image in this class
	void gaussianBlurTrackBar()
	{
	
		namedWindow( "GaussianBlur" );
		createTrackbar( "xKSize", "GaussianBlur", &xKSize, 100, 0 );
		createTrackbar( "yKSize", "GaussianBlur", &yKSize, 100, 0 );
		createTrackbar( "sigmaX", "GaussianBlur", &sigmaX, 100, 0 );
		createTrackbar( "sigmaY", "GaussianBlur", &sigmaY, 100, 0 );
		

	}	// End of the function gaussianBlurTrackBar

	~ThresholdTheImage()
	{
		imageHSV.release();
		destroyAllWindows();
	}
	
private:
	Mat imageHSV;	//image HSV for thesholding red

	//Data Members fot the HSV range in thresholding;
	int hLower, sLower, vLower;
	int hUpper, sUpper, vUpper;

	//Data members for Gaussian Blur parameters
	int xKSize, yKSize;
    int sigmaX, sigmaY;	


};		//end of class Threshold the image

class HoughCircle
{
public:	

	HoughCircle()
	{
		//Initialization of parameters for the index of co-ordinate array
		ini = 0;
		fin = 1;
		
	}

	//Function to find the circle in the thresolded image
	void findCircle( Mat &sourceGray )
	{		
		
		//Applying Hough Transform to find the circle
		HoughCircles( sourceGray, circles, CV_HOUGH_GRADIENT, 2, sourceGray.rows * 2, 200, 8, 2, 10 );
		
	}	//end of the function findCircle

	//Function to plot ROI or circle in the image
	void plotCircle( Mat &image )
	{		 
		//Checking the conditon for the even frame
		if( ( circles.size() > 0 ) && ( pointCounter % 2 == 0 ) )
		{
			//Rounding up the values from the Hough Circle centre to nearest integer value 
			Point centreEven ( cvRound( circles[0][0] ), cvRound( circles[0][1] ) );
			
			//Storing x and y co-ordintes in 2x2 array point for even circles
			point[0][0] = cvRound( circles[0][0] );
			point[0][1] = cvRound( circles[0][1] );
			
			/*Rounding up the radius from the Hough Circle to nearest
				integer value for setting in ROI in actual frame */
			radius = cvRound( circles[0][2] );

			//Drawing inner circle in actual frame to show ROI ( blue )
			circle( image, centreEven, 3, Scalar( 255, 0, 0 ), -1, 8, 0 );

			// Drawing outer circle of ROI ( red )
			circle( image, centreEven, radius, Scalar( 0, 0, 255 ), 2, 8, 0 );	
			
		}	//End of if
		    
			//Condition check for odd numbers of frame
			else if( ( circles.size() > 0 ) )
		{
			//Rounding up the values from the Hough Circle centre to nearest integer value 
			Point centreOdd ( cvRound( circles[0][0] ), cvRound( circles[0][1] ) );
			
			//Storing x and y co-ordintes in 2x2 array point for even circles
			point[1][0] = cvRound( circles[0][0] );
			point[1][1] = cvRound( circles[0][1] );
			
			/*Rounding up the radius from the Hough Circle to nearest
				integer value for setting in ROI in actual frame */
			radius = cvRound( circles[0][2] );

			//Drawing inner circle of ROI ( blue )
			circle( image, centreOdd, 3, Scalar( 255, 0, 0 ), -1, 8, 0 );

			//Drawing Outer circle of ROI ( red )
			circle( image, centreOdd, radius, Scalar( 0, 0, 255 ), 2, 8, 0 );						
			
		}	//end of else if

		   /* Increasing counter value so as to make possible 
				to check the even or odd nummber of frames*/
			pointCounter++;

	}	//end of the function plotCircle

	
	//Function to draw line onto the whitebaord
	void drawLine( Mat &board )
	{
	   //opencv's function to plot the circle	   
	   line( board, Point( point[ini%2][0], point[ini%2][1] ), Point( point[fin%2][0], point[fin%2][1] ), Scalar( 0, 0 , 0 ), 1 , 8, 0  );
	   
	  /* Increase the counter which will help to plot the line by 
		   finding the initial and final set of points fron 2x2 array*/

	   ini++;  // Termed initial 
	   fin++;	//Termed final

	}	//end of the function drawline

	
private:
	Mat sourceGray;	//for storing Grayscale image

protected:
    vector<Vec3f> circles;	//for storing parameters od circle detected

	Point centreOdd;	//center co-ordinates of circle for odd frame
	Point centreEven;	//center co-ordinates of circle for even frame
	
	int radius;	//radius of circle

	int point[2][2]; //2x2 array to store the circle radius	

	int ini;    // Initial point index in 2x2 array of points
	int fin;	// Final point index in 2x2 array of points
	

};	//end of class HoughCircle


//main function
int main()
{
	ThresholdTheImage frameForThresholding;
	HoughCircle frameForCircle;
	
	Mat captureFrame;	
    Mat wB = imread( "one.jpg" );
	
	cvtColor( wB, wB, CV_BGR2GRAY );

	//name the title of the windows to show the videofeed and plot area
	namedWindow( "Actual" );
	namedWindow( "Segmented" );
	
		
	bool until = true;

	//Creating Trackbar for thresholding
	frameForThresholding.trackBarsForHSVLimits();

	//Creating Trackbar for Gaussian Blur in thresholdedimage
	 frameForThresholding.gaussianBlurTrackBar();

	 //frameForCircle.trackbarsForPenSize();

	//Seleting which device to use to capture
	VideoCapture deviceCapture( deviceName );
	
	//check whether the hardware available or not
	if( !deviceCapture.isOpened() )
		return -1;
	
	//Start calibartion of camera
	loop( until )
	{
		deviceCapture >> captureFrame;		

		//Thresholding Red
		Mat thresholdedImage = frameForThresholding.thresholdRedimage( captureFrame );
		
		imshow( "Segmented", thresholdedImage );

		//finds the circle
		frameForCircle.findCircle( thresholdedImage );
		
		//plot contour of detected circle
		frameForCircle.plotCircle( captureFrame );	
		
		imshow( "Actual", captureFrame );
						
		//free the memory of captureFrame and thresholdedImage
		captureFrame.release();
		thresholdedImage.release();
		
		if( waitKey( 27 ) >= 0 )
			until = false;
		

	}	//end of loop for video feed

	destroyAllWindows();
	namedWindow( "Whiteboard" );
	until =  true;
		
	//running infinite loop to capture the videofeed frame by frame and plot the locus
	loop( until )
	{
		deviceCapture >> captureFrame;		

		//Thresholding Red
		Mat thresholdedImage = frameForThresholding.thresholdRedimage( captureFrame );
		
		//finds the circle
		frameForCircle.findCircle( thresholdedImage );
		
		//plot contour of detected circle
		frameForCircle.plotCircle( captureFrame );	
		
		if( frameCounter != 0 )
		frameForCircle.drawLine( wB );
	    
	    imshow( "Whiteboard", wB );
				
		//free the memory of captureFrame and thresholdedImage
		captureFrame.release();
		thresholdedImage.release();
		
		if( waitKey( 27 ) >= 0 )
			until = false;
		frameCounter++;

	}	//end of loop for video feed

	//free the memory of deviceCapture
	deviceCapture.release();
	wB.release();

	//free the memory of all windows previously named
	destroyAllWindows();

}	//end of main