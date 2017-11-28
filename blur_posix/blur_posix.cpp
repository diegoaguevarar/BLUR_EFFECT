#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cstdlib>
#include <pthread.h>

using namespace std;
using namespace cv;

//numero de hilos
int NUM_THREADS;
//numero de hilos que han terminado el trabajo asignado
int contador = 0;
//tamaño del kernel de efecto
int dis;
//filas asignadas a cada hilo
int distancia;
//imagen original
Mat src1;
//imagen editada
Mat edit;


//proceso asignado a cada hilo
void *Efecto_borroso(void *threadid) {
  int i,j,k,r_aux,g_aux,b_aux,can;

  //id del hilo
  long tid;
  tid = (long)threadid;

  //proceso a ejecutar por todos los hilos menos el último
	if(tid<(NUM_THREADS-1)){
    for (i=tid*distancia; i<(tid+1)*distancia; i++){
      for (j=0; j<src1.cols; j++){
        r_aux = 0;
        g_aux = 0;
        b_aux = 0;
        can = 0;
        for (k=(-1*dis); k<=dis; k++){
          r_aux = r_aux + int(src1.at<Vec3b>(i,j+k)[0]) + int(src1.at<Vec3b>(i+k,j)[0]);
          g_aux = g_aux + int(src1.at<Vec3b>(i,j+k)[1]) + int(src1.at<Vec3b>(i+k,j)[1]);
          b_aux = b_aux + int(src1.at<Vec3b>(i,j+k)[2]) + int(src1.at<Vec3b>(i+k,j)[2]);
          can += 2;
        }
        edit.at<Vec3b>(i,j)[0] = int(r_aux / can );
        edit.at<Vec3b>(i,j)[1] = int(g_aux /can );
        edit.at<Vec3b>(i,j)[2] = int(b_aux /can );
      }
    }
  }

  //proceso a ejecutar por el último hilo
  else{

    for (i=tid*distancia; i<(tid+1)*distancia-dis; i++){
      for (j=0; j<src1.cols; j++){
        r_aux = 0;
        g_aux = 0;
        b_aux = 0;
        can = 0;
        for (k=(-1*dis); k<=dis; k++){
          r_aux = r_aux + int(src1.at<Vec3b>(i,j+k)[0]) + int(src1.at<Vec3b>(i+k,j)[0]);
          g_aux = g_aux + int(src1.at<Vec3b>(i,j+k)[1]) + int(src1.at<Vec3b>(i+k,j)[1]);
          b_aux = b_aux + int(src1.at<Vec3b>(i,j+k)[2]) + int(src1.at<Vec3b>(i+k,j)[2]);
          can += 2;
        }
        edit.at<Vec3b>(i,j)[0] = int(r_aux / can );
        edit.at<Vec3b>(i,j)[1] = int(g_aux /can );
        edit.at<Vec3b>(i,j)[2] = int(b_aux /can );
      }
    }

    //ciclo encargado de aplicar el efecto a la margen inferior
    for (i=(tid+1)*distancia-dis; i<src1.rows; i++){
      for (j=0; j<src1.cols; j++){
        r_aux = 0;
        g_aux = 0;
        b_aux = 0;
        can = 0;
        for (k=(-1*dis); k<=dis; k++){
          if((j+k)>src1.cols){
            r_aux = r_aux + int(src1.at<Vec3b>(i,j)[0]);
            g_aux = g_aux + int(src1.at<Vec3b>(i,j)[1]);
            b_aux = b_aux + int(src1.at<Vec3b>(i,j)[2]);
          }
          else{
            r_aux = r_aux + int(src1.at<Vec3b>(i,j+k)[0]);
            g_aux = g_aux + int(src1.at<Vec3b>(i,j+k)[1]);
            b_aux = b_aux + int(src1.at<Vec3b>(i,j+k)[2]);
          }
          if(i+k>src1.rows){
            r_aux = r_aux + int(src1.at<Vec3b>(i,j)[0]);
            g_aux = g_aux + int(src1.at<Vec3b>(i,j)[1]);
            b_aux = b_aux + int(src1.at<Vec3b>(i,j)[2]);
          }
          else{
            r_aux = r_aux + int(src1.at<Vec3b>(i+k,j)[0]);
            g_aux = g_aux + int(src1.at<Vec3b>(i+k,j)[1]);
            b_aux = b_aux + int(src1.at<Vec3b>(i+k,j)[2]);
          }
          can += 2;
        }
        edit.at<Vec3b>(i,j)[0] = int(r_aux / can );
        edit.at<Vec3b>(i,j)[1] = int(g_aux /can );
        edit.at<Vec3b>(i,j)[2] = int(b_aux /can );
      }
    }

  }

  contador = contador + 1;
  pthread_exit(NULL);
}

int main ( int argc, char** argv ) {
  //lectura de la imagen que será procesada
  src1 = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);
  edit = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);

  //lectura del tamaño del kernel a realizar
  dis = atoi(argv[2]);

  //lectura de la cantidad de hilos a emplear
  NUM_THREADS = atoi(argv[3]);

  //cantidad de filas que se le asignaran a cada hilo dadas por: filas totales / número de hilos.
  distancia = int((src1.rows)/NUM_THREADS);

  //declaración de los n hilos
  pthread_t threads[NUM_THREADS];
  int rc;
  int i;

  //Creación de los n hilos
  for( i = 0; i < NUM_THREADS; i++ ) {
    rc = pthread_create(&threads[i], NULL, Efecto_borroso, reinterpret_cast<void*>(i));

    //error
    if (rc) {
      cout << "Error:unable to create thread," << rc << endl;
      exit(-1);
    }

  }
  //impresión de n hilos y tiempo total de ejecucion
  //cout<<"NUMERO DE HILOS:"<<NUM_THREADS<<endl;
  //cout<<"TIEMPO: ";

  //espera que todos los hilos terminen su ejecución para guardar la imagen obtenida
  while(contador != NUM_THREADS){
  }
  imwrite( "edicion.jpg", edit );


  pthread_exit(NULL);
}
