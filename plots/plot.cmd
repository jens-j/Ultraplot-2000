filename = "pid12"
set datafile separator ","
set terminal png size 1920,1024 enhanced font "Helvetica,20"
set output filename.".png"
plot filename.".csv" using 1:2 with lines, filename.".csv" using 1:3 with lines, filename.".csv" using 1:4 with lines