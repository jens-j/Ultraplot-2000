set datafile separator ","
set terminal png size 1920,1024 enhanced font "Helvetica,20"
set output 'right_fast.png'
plot "right_fast.csv" using 1:2 with lines