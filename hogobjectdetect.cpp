//detect objects using a SVM trained using hog features from positive and negative images of concerned objects
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml.hpp>
#include <iostream>
#include <stdarg.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <fstream>
#include <string>
#include <time.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ml/ml.hpp"

using namespace cv;
using namespace cv::ml;
using namespace std;

int help();
void svmdetector(Ptr<SVM>& svm,vector< float >& hogdetector);
void drawlocations(Mat& draw,const vector< Rect >& objectlocations);

int main(int argc,char** argv)
{
     cout<<"argc : "<<argc<<endl;
     int option=help();

     cout<<"Declaring variables.."<<endl;
     int esc=27;
     Mat img,grayimg,draw;
     vector< Rect > objectlocations;
     string trainedsvmname=argv[1];

     cout<<"Loading trained SVM file.."<<endl;
     //Load trained SVM xml file
     Ptr<SVM> svm=SVM::load(argv[1]);
     //Ptr<SVM> svm;
     //svm=StatModel::load<SVM>(argv[1]);
     //svm->load(argv[1]);
     cout<<"Declaring HOG descriptor.."<<endl;
     HOGDescriptor hog(Size(64,48),Size(8,8),Size(4,4),Size(4,4),9);
     hog.winSize=Size(64,48);
     
     //setting trained SVM to hog
     //getting support vectors
     //Mat sv=svm->getSupportVectors();
     //int svtot=sv.rows;
     //cout<<endl<<svtot<<endl;

     cout<<"Calling svmdetector.."<<endl;
     vector < float >  hogdetector;
     svmdetector(svm,hogdetector);
     cout<<"Back to main.."<<endl;
     hog.setSVMDetector(hogdetector);
     cout<<"Option is : "<<option<<endl;

     if((option!=1)&&(option!=2))
     {
      cout<<"First if\tOption is : "<<option<<endl;
      exit(0);
     }

     if(option==2){
     cout<<"Option is : "<<option;//<<endl;
     //starting webcam
     cout<<endl<<"Starting webcam now.."<<endl;
     VideoCapture vdo(0);
     while(1)
     {
          bool imgread=vdo.read(img);
          if(!imgread) 
          {
           cout<<"Cam not opening";
           return -1;
          }
          imshow("LiveFeed",img);

          /*draw=img.clone();
          objectlocations.clear();

          hog.detectMultiScale(img,objectlocations);
          drawlocations(draw,objectlocations);

          objectlocations.clear();
          
          //show detected objects
          imshow("Detected",draw);
      */
          if(waitKey(30)==esc) break;
     }}

     if(option==1)
     {
          cout<<"Option is : "<<option<<endl;
          cout<<"argc : "<<argc<<endl;
          if(argc!=3)
          { cout<<"Type the path to the image as the third argument in the command line"<<endl; return -1; }
          img=imread(argv[2]); 
          resize(img,img,Size(64,48));
          //cvtColor(img,grayimg,CV_BGR2GRAY);
          imshow("Original",img);
          draw=img.clone();
          cout<<"Cloned image.."<<endl;
          objectlocations.clear();

          cout<<"Calling detectMultiScale.."<<endl;
          hog.detectMultiScale(img,objectlocations,0.0,Size(4,4),Size(),1.01,0.1);
          cout<<"Calling drawlocations.."<<endl;
          drawlocations(draw,objectlocations);

          //objectlocations.clear();
          
          //show detected objects
          imshow("Detected",draw);
      } 

     cout<<"end"<<endl;
     waitKey(0);
     return -1;
}

int help()
{
     int option;
     cout<<endl<<"Press 1 to detect object in an image and 2 to detect in a live feed using webcam : ";
     cin>>option;
     return option;
}

void svmdetector(Ptr<SVM>& svm,vector< float >& hogdetector)
{
    cout<<"Getting support vectors.."<<endl;
    //getting support vectors
     Mat sv=svm->getSupportVectors();
     int svtot=sv.rows;
     cout<<endl<<svtot<<endl;
     Mat alpha,svidx;
     double rho=svm->getDecisionFunction(0,alpha,svidx);

     CV_Assert(alpha.total()==1 && svidx.total()==1 && svtot==1);
     CV_Assert((alpha.type()==CV_64F && alpha.at<double>(0)==1.) || (alpha.type()==CV_32F && alpha.at<float>(0)==1.f));
     CV_Assert(sv.type()==CV_32F);

     hogdetector.clear();
     hogdetector.resize(sv.cols+1);
     memcpy(&hogdetector[0], sv.ptr(),sv.cols*sizeof(hogdetector[0]));
     hogdetector[sv.cols]=(float)-rho;
}

void drawlocations(Mat& draw,const vector< Rect >& objectlocations)
{
     if(objectlocations.empty()) exit(0);

     cout<<"Inside draw function.."<<endl;
     imshow("In draw function",draw);

     vector< Rect >::const_iterator start   =  objectlocations.begin();
     vector< Rect >::const_iterator end     =  objectlocations.end();

     for( ; start!=end ; start++)
          rectangle(draw,*start,Scalar(0,255,0),2);
     cout<<objectlocations.size();
     /*for(int i=0;i<objectlocations.size();i++)
          rectangle(draw,objectlocations[i],Scalar(0,255,0),5);*/
     //cvtColor(draw,draw,CV_GRAY2BGR);
     resize(draw,draw,Size(400,400));
     imshow("Detectedfirst",draw);

}
