#include "opencv2/highgui/highgui.hpp"
#include <omp.h>
#include <iostream>

using namespace cv;
using namespace std;

Mat img;
Mat edit;
int radio, NUM_THREADS;
//Funcion main
int main( int argc, char** argv )
{
  int i, j;

  //Abrir las imagenes y guardarlas en memoria
  img = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);
  edit = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);

  radio=atoi(argv[2]);

	NUM_THREADS = atoi(argv[3]);
  int width=img.cols;
  int height=img.rows;

  #pragma omp parallel for schedule(guided) num_threads(NUM_THREADS) shared (edit,img) collapse(2)
  for (i=0; i<height; i++){
   for (j=0; j<width; j++){
     int aux_r=0;
     int aux_g=0;
     int aux_b=0;
     int aux_div=0;
     for (int a=-radio; a<=radio; a++){
       if(i+a<0 || i+a>height){
         aux_r+= img.at<cv::Vec3b>(i-a,j)[0];
         aux_g+= img.at<cv::Vec3b>(i-a,j)[1];
         aux_b+= img.at<cv::Vec3b>(i-a,j)[2];
       }
			 else{
         aux_r+= img.at<cv::Vec3b>(i+a,j)[0];
         aux_g+= img.at<cv::Vec3b>(i+a,j)[1];
         aux_b+= img.at<cv::Vec3b>(i+a,j)[2];
       }
       if(j+a<0 || j+a>width){
         aux_r+= img.at<cv::Vec3b>(i,j-a)[0];
         aux_g+= img.at<cv::Vec3b>(i,j-a)[1];
         aux_b+= img.at<cv::Vec3b>(i,j-a)[2];
       }
       else{
         aux_r+= img.at<cv::Vec3b>(i,j+a)[0];
         aux_g+= img.at<cv::Vec3b>(i,j+a)[1];
         aux_b+= img.at<cv::Vec3b>(i,j+a)[2];
       }
			 aux_div+=2;
     }
   edit.at<cv::Vec3b>(i,j)[0]=aux_r/aux_div;
   edit.at<cv::Vec3b>(i,j)[1]=aux_g/aux_div;
   edit.at<cv::Vec3b>(i,j)[2]=aux_b/aux_div;
   }
  }
  //Guardar la imagen
  imwrite("editada.jpg", edit);
  return 0;
}
