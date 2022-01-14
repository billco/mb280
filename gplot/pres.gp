
set timefmt "%m/%d/%Y %H:%M:%S"
set datafile separator ","
set xdata time
set xrange [*:*]
#set xlabel "Time"
set autoscale y  
set format x "%a %H:%M:%S"  
set xtics time
set xtics rotate by -85                
set autoscale x  
#show timefmt

#set terminal pngcairo size 1200,600 enhanced font " font,4"
#set output 'output.png'

set terminal wxt size 1200,600
#show terminal

set xtics rotate
set multiplot
set tmargin 4
set bmargin  6
set rmargin 2
set lmargin 10
set size 0.8, 0.90
set grid

##? bind "ctrl-a"          # shows the binding for ctrl-a
   ##?bind 


set tmargin 2
set xtics rotate by -25                
set ylabel "Presssure" 
set autoscale y  
unset key

plot "../bin/data/history4.csv" using 1:4 with lines linetype 1 linewidth 1
  

#
