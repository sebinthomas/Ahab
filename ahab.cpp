// ahab.cpp : Defines the entry point fo r the console application.
//

#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include<math.h>
#include<cstdlib>
#include <iostream>

using namespace std;
// Class initialization for Frame Processing part
class frameProcessor
{
   IplImage * img1;
   IplImage * img2;

   IplImage * processedImg;
   IplImage * processedImgGray;

public:
	
	IplImage * procImg(void);
	IplImage * procImgGray(void);
	void clear(void);
    
	void compareImages(IplImage * img1, IplImage * img2);
    int getWhiteInArea(int startX, int startY, int w, int h);
	frameProcessor(void);
	~frameProcessor(void);
};


// Main Funcs
IplImage* game=0; // game image
CvCapture* cam=0; // Camera Image
IplImage* frame=0;
IplImage* frame1=0;
int draw(int,int,int,int); // draws the ball and line(TO.do)
int ball_coll_x (int,int); // ball collision x
int ball_coll_y (int,int,int,int,int);// Ball Collision y


int main()
{
cam=cvCreateCameraCapture(0); // capture from camera
frameProcessor fp;

CvPoint pnt,mp,paddle; // init variables
mp.x=1;
mp.y=0;
pnt.x=320; 
pnt.y=200;
paddle.x=320;
paddle.y=400;
int lcnt=0;int rcnt=0,m=0;
cvNamedWindow("ahab",1);
cvNamedWindow("cam_footage",CV_WINDOW_AUTOSIZE );

for(int i=0;;i++) // game loop
{
frame = cvCloneImage(cvQueryFrame(cam));
frame1=cvCloneImage(cvQueryFrame(cam));
if( !frame || !frame1 ) break;
fp.compareImages(frame,frame1);
lcnt=fp.getWhiteInArea(0,0,(frame->width)/2,frame->height);
rcnt=fp.getWhiteInArea ((frame->width)/2,0,frame->width,frame->height);

cvShowImage( "cam_footage", frame );


	if(i>=50)          // Randomize movement
	{i=0;
	mp.x=(rand()%2+1>1)?-1:1;
	mp.y=(rand()%2+1>1)?-1:1; 

	} 


mp.x=ball_coll_x(pnt.x,mp.x);
pnt.x+=10*mp.x ;               //checks collision in x
mp.y=ball_coll_y(pnt.y,pnt.x,mp.y,paddle.x,paddle.y);// checks in y

if((lcnt>500) || (rcnt>500))
{
if(lcnt>rcnt)
{
		m=10;
	
} //tesst
else if(lcnt<rcnt)
{
		m=-10;
	
}
} //test
	
if(paddle.x<0){paddle.x=0;}
else if((paddle.x+128)>640){paddle.x=512;}
paddle.x+=m;
	

cout<<lcnt<<"  "<<rcnt<<endl;
pnt.y+=10*mp.y; 
draw(pnt.x,pnt.y,paddle.x,paddle.y); 
cvReleaseImage(&frame);
cvReleaseImage(&frame1);
}


cvReleaseCapture(&cam);
cvDestroyWindow( "cam_footage" );
cvDestroyWindow( "ahab" );
return 0;
}



int draw(int x1,int y1,int px1,int py1)
{
CvPoint pnt,paddle,paddle2;
pnt.x=x1;
pnt.y=y1;
paddle.x=px1;
paddle.y=py1;
paddle2.x=px1+128;
paddle2.y=py1;
cvReleaseImage( &game );
game=cvCreateImage( cvSize(640,480), 8, 3 );
cvCircle(game,pnt,32,CV_RGB(2,181,255),-1,CV_AA,0);
cvLine(game,paddle,paddle2,CV_RGB(2,181,255),2,8,0);
char c=cvWaitKey(20);
cvShowImage( "ahab", game );
return 0;
}
int ball_coll_x (int pnt,int prev)
{
	int mp;
	if((pnt-32)<0)
	{mp=1;}
	else if((pnt+32)>640)
	{mp=-1;}
	else
	{mp=prev;}
	return mp;
	}

int ball_coll_y (int pnt,int bx,int prev,int px,int py)
{
int mp;
if((pnt+32)>480)
	{mp=-1;}
	else if((pnt-32)<0)
	{mp=1;}
	else if(((pnt+32)>=py)&&((bx>=px)&&(bx<=px+128)))
	{mp=-1;}
	else
	{mp=prev;}
	return mp;
	
}



// Class Functions for  Frame Processing
// Processing Funcs
IplImage * frameProcessor::procImg()
{
   return this->processedImg; //Return processed image (colored)
}
IplImage * frameProcessor::procImgGray()
{
   return this->processedImgGray; //Return processed image (grayscale)
}

//Function to get white pixels in grayscale image area
int frameProcessor::getWhiteInArea(int startX, int startY, int w, int h)
{
   if (this->processedImgGray == 0) return -1; //If there's no grayscale processed image, return error.
   
   //Go back to bounds
   if (startX < 0) startX = 0;
   if (startX >= this->processedImgGray->width) return -1;

   if (startY < 0) startY = 0;
   if (startY >= this->processedImgGray->height) return -1;
   
   //Initialise counter
   int whiteCount = 0;
   
   //Loop through area
   for (int x = startX; ((x <= startX+w) && (x < this->processedImgGray->width)); x++) 
   {
      for (int y = startY; ((y <= startY+h) && (y < this->processedImgGray->height)); y++)
	  {
		  int tmp = ((uchar*)(this->processedImgGray->imageData + this->processedImgGray->widthStep*y))[x];
		  if (tmp == 255) whiteCount++; //If it's white -> add to whitecount.

		 

	  }
   }
   
   return whiteCount;
}
void frameProcessor::compareImages(IplImage * img1, IplImage * img2) //Compare twho images (frame)
{
    this->clear(); //Clear everything

	//Check if it's images
	if ((img1 == 0) || (img2 == 0)) { fprintf(stderr, "One/both of images to compare are null\n"); return; }
    
	//Clone image in order not to modify the original
	this->img1 = cvCloneImage(img1); 
	this->img2 = cvCloneImage(img2);

	//Create processed image buffer
	this->processedImg = cvCreateImage(cvGetSize(this->img1), 8, 3); 
    
    //Blur images to get rid of camera noise
	cvSmooth(this->img1, this->img1, CV_BLUR, 3); 
	cvSmooth(this->img2, this->img2, CV_BLUR, 3); 

	//Calc absolute difference
	cvAbsDiff(this->img1, this->img2, this->processedImg);

	//Create gray image buffer
	this->processedImgGray = cvCreateImage(cvGetSize(this->processedImg), 8, 1);
    
    //Convert colored image to grayscale
	cvCvtColor(this->processedImg, this->processedImgGray, CV_RGB2GRAY);
	
	//Perform binary treshold filter on image to leave only white and black pixels
	cvThreshold(this->processedImgGray, this->processedImgGray, 30, 255, CV_THRESH_BINARY); 
}
void frameProcessor::clear() //Clear everything (prepare for reuse)
{
   cvReleaseImage(&this->img1);
   cvReleaseImage(&this->img2);
   cvReleaseImage(&this->processedImg);
   cvReleaseImage(&this->processedImgGray);

   this->img1 = 0;
   this->img2 = 0;
   this->processedImg = 0;
   this->processedImgGray = 0;
}
frameProcessor::frameProcessor(void)
{
   this->img1 = 0;
   this->img2 = 0;
   this->processedImg = 0;
   this->processedImgGray = 0;
}

frameProcessor::~frameProcessor(void)
{
	this->clear();
}
