g++ -fopenmp `pkg-config --cflags opencv` ConOMP.cpp `pkg-config --libs opencv` -o exec.out

TIMEFORMAT=%R
rm -r datos.txt

echo "720P:"
tam_kernel=20
while [  $tam_kernel -lt 70 ]; do
  echo "procesando..." $tam_kernel
  echo "" >> datos.txt
  echo "o_720p_kernel_"$tam_kernel" = [" >> datos.txt
  n_hilos=0
  while [  $n_hilos -lt 12 ]; do
    let n_hilos=n_hilos+1
    { time ./exec.out 720.jpg $tam_kernel $n_hilos >/dev/null 2>&1; } 2>> datos.txt
  done
  let tam_kernel=tam_kernel+20
  echo "];" >> datos.txt
done

echo "" >> datos.txt
echo "" >> datos.txt
echo "1080P:"
tam_kernel=20
while [  $tam_kernel -lt 70 ]; do
  echo "procesando..." $tam_kernel
  echo "" >> datos.txt
  echo "o_1080p_kernel_"$tam_kernel" = [" >> datos.txt
  n_hilos=0
  while [  $n_hilos -lt 12 ]; do
    let n_hilos=n_hilos+1
    { time ./exec.out 1080.jpg $tam_kernel $n_hilos >/dev/null 2>&1; } 2>> datos.txt
  done
  let tam_kernel=tam_kernel+20
    echo "];" >> datos.txt
done

echo "" >> datos.txt
echo "" >> datos.txt
echo "4k:"
tam_kernel=20
while [  $tam_kernel -lt 70 ]; do
  echo "procesando..." $tam_kernel
  echo "" >> datos.txt
  echo "o_4k_kernel_"$tam_kernel" = [" >> datos.txt
  n_hilos=0
  while [  $n_hilos -lt 12 ]; do
    let n_hilos=n_hilos+1
    { time ./exec.out 4k.jpg $tam_kernel $n_hilos >/dev/null 2>&1; } 2>> datos.txt
  done
  let tam_kernel=tam_kernel+20
    echo "];" >> datos.txt
done
