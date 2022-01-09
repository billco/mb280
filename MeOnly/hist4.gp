#01/09/2022 00:00:00, 66.8, 75.2,1014.5

set timefmt "%m/%d/%Y %H:%M:%S"
set datafile separator ","
set xdata time
#set xrange ["01/09/2022 00:00:00":"01/09/2022 23:59:00"]
set xrange [*:]

set xlabel "Time"
set ylabel "Humdity" 
set autoscale y  
set format x "%m/%d/%Y %H:%M:%S"  
set xtics time
set xtics rotate by -85                
set autoscale x  
     show timefmt

set terminal wxt size 1200,600
show terminal
#plot "history4.csv" using 1:2 with linespoints linetype 6 linewidth 2, \
#    "history4.csv" using 1:3 with linespoints linetype 7 linewidth 2, \
#    "history4.csv" using 1:4 with linespoints linetype 8 linewidth 2 
plot "../bin/data/history4.csv" using 0:3 with linespoints linetype 8 linewidth 1
