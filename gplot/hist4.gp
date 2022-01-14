#01/09/2022 00:00:00, 66.8, 75.2,1014.5

set timefmt "%m/%d/%Y %H:%M:%S"
set datafile separator ","
set xdata time
set xrange [*:*]
set xlabel "Time"
set ylabel "Humdity" 
set autoscale y  
set format x "%a %H:%M:%S"  
set xtics time
set xtics rotate by -85                
set autoscale x  
#show timefmt

set terminal wxt size 1200,600
#show terminal
plot "../bin/data/history4.csv" using 1 :3 with linespoints linetype 1 linewidth 1, \
#	"../bin/data/history4.csv" using 1 :3 with linespoints linetype 2 linewidth 2

