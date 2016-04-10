infilename = "pid.csv"
outfilename = "pid_filter_"
outfilenumber = 14
set datafile separator ","
set terminal png size 1920,1024 enhanced font "Helvetica,20"
set output outfilename.outfilenumber.".png"
plot infilename using 1:2 with lines