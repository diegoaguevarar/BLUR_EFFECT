nvcc `pkg-config --cflags opencv` -arch blur_cuda.cu -lpthread `pkg-config --libs opencv` -o exec_cuda.out


tam_kernel=40  		#Tamaño del kernel
n_bloques=11   		#Cantidad de bloques a usar
n_hilos=11   		#Cantidad de hilos a usar
imagen=1080.jpg	 	#Nombre de la imagen

echo "Ejecutando blur_cuda....."
./exec_cuda.out $imagen $tam_kernel $n_bloques $n_hilos
echo "Ejecución completada, se ha aplicado el efecto a" $imagen ",empleando" $n_bloques "bloques," $n_hilos "hilos y un tamaño de kernel de" $tam_kernel"."
