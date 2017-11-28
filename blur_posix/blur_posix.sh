g++ `pkg-config --cflags opencv` blur_posix.cpp `pkg-config --libs opencv` -lpthread -o exec_posix.out


tam_kernel=40  		#Tamaño del kernel
n_hilos=11   		#Cantidad de hilos a usar
imagen=1080.jpg	 	#Nombre de la imagen

echo "Ejecutando blur_posix....."
./exec_posix.out $imagen $tam_kernel $n_hilos
echo "Ejecución completada, se ha aplicado el efecto a" $imagen ",empleando" $n_hilos "hilos y un tamaño de kernel de" $tam_kernel"."
