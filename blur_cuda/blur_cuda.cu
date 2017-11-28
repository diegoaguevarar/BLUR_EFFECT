//librerias
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <cuda.h>

using namespace std;
using namespace cv;

//imagen original
Mat src1;
//imagen editada
Mat edit;

// CUDA API error checking macro
static void handleError( cudaError_t err, const char *file,  int line ) {
    if (err != cudaSuccess) {
        printf( "%s in %s at line %d\n", cudaGetErrorString( err ),  file, line );
        exit( EXIT_FAILURE );
    }
}
#define cudaCheck( err ) (handleError( err, __FILE__, __LINE__ ))

__global__ void stencil_1d(int *in , int *out, int di, int repetir){
  //index del hilo actual
  int gindex = repetir +threadIdx.x + (blockIdx.x * blockDim.x);
  int aux = 0;

  //suma de todos los valores desde di a la izquierda hasta di a la derecha del pixel en cuestion
  for (int offset = -di ; offset <= di ; offset++){
    if(gindex+offset>0)
      aux += in[gindex+offset];
    else
      aux += in[gindex-offset];
  }


  //promedio de pixeles sumados
  aux = int(aux/(di*2+1));

  //guardado del valor a enviar al host del nuevo valor del pixel
  out[gindex] = aux;


}

int main ( int argc, char** argv ) {

  //lectura de la imagen que será procesada
  src1 = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);
  edit = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);
  //lectura del tamaño del kernel a realizar
  int distorcion = atoi(argv[2]);
  //lectura de la cantidad de cores que serán lanzados
  int NUM_THREADS = atoi(argv[4]);

  //lectura de la cantidad de cores que serán lanzados
  int NUM_BLOCKS = atoi(argv[3]);

  //variables de host
  int k,j,i, h_red_in[src1.cols],h_red_out[src1.cols],repetir;
  //variables de device
  int *d_red_in, *d_red_out;


  //Reserva de recursos en device
  cudaMalloc( (void **) &d_red_in, src1.cols * sizeof(int));
  cudaMalloc( (void **) &d_red_out, src1.cols * sizeof(int));


  //k recorre fila por fila
  for(k=0;k<src1.rows;k++){
    //i recorre los valores de rgb e.d 0=r 1=g 2=b
    for(i=0;i<=2;i++){
      //asigna los valores de la fila actual en el host
      for(j=0;j<src1.cols;j++)
        h_red_in[j] = int(src1.at<Vec3b>(k,j)[i]);

      //envia los valores de la fila actual del host al device
      cudaCheck( cudaMemcpy( d_red_in, h_red_in, src1.cols * sizeof(int), cudaMemcpyHostToDevice));

      for(repetir=0;repetir<=src1.cols/(NUM_BLOCKS*NUM_THREADS);repetir++)
        stencil_1d<<<NUM_BLOCKS,NUM_THREADS>>> (d_red_in,d_red_out,distorcion,repetir*(NUM_BLOCKS*NUM_THREADS));

      //guarda en el host los valores generados por el stencil
      cudaMemcpy( h_red_out, d_red_out, src1.cols * sizeof(int), cudaMemcpyDeviceToHost);

      //recorre la fila actual y le asigna los nuevos valores rgb
      for(j=0;j<src1.cols;j++)
        edit.at<Vec3b>(k,j)[i] = h_red_out[j];

    }
  }

  //k recorre fila por columna
  for(k=0;k<src1.cols;k++){
    //i recorre los valores de rgb e.d 0=r 1=g 2=b
    for(i=0;i<=2;i++){
      //asigna los valores de la columna actual en el host
      for(j=0;j<src1.rows;j++)
        h_red_in[j] = int(src1.at<Vec3b>(j,k)[i]);

      //envia los valores de la columna actual del host al device
      cudaCheck( cudaMemcpy( d_red_in, h_red_in, src1.rows * sizeof(int), cudaMemcpyHostToDevice));

      for(repetir=0;repetir<=src1.rows/(NUM_BLOCKS*NUM_THREADS);repetir++)
        stencil_1d<<<NUM_BLOCKS,NUM_THREADS>>> (d_red_in,d_red_out,distorcion,repetir*(NUM_BLOCKS*NUM_THREADS));

      //guarda en el host los valores generados por el stencil
      cudaMemcpy( h_red_out, d_red_out, src1.rows * sizeof(int), cudaMemcpyDeviceToHost);

      //recorre la columna actual y le asigna los nuevos valores rgb a partir de el valor obtenido con el valor previo
      for(j=0;j<src1.rows;j++)
        edit.at<Vec3b>(j,k)[i] = (int(edit.at<Vec3b>(j,k)[i])+h_red_out[j])/2;

    }
  }

  //libera la memoria del device
  cudaFree(d_red_out);
  cudaFree(d_red_in);

  //genera la imagen
  imwrite( "edicion.jpg", edit );

  return 0;
}
