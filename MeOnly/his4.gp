set timefmt '%m/%d/-%Yd %H:%M:%S'
set datafile separator ","
set xdata time
set format x "%m-%d\n%H:%M"
set xlabel "Time"
set ylabel "Traffic" 
set autoscale y  
plot "history4.csv"
