##

set terminal wxt size 1200,600

show terminal
 
set timefmt "%m/%d/%Y %H:%M:%S"
set datafile separator ","
set xdata time
set xtics time
set autoscale x  

set xrange [*:*]
#
#set autoscale y  
set format x "%a %H:%M:%S"  
set yrange [*:*]
#show timefmt

set xtics rotate
set multiplot
set tmargin 4
set bmargin  6
set rmargin 2
set lmargin 10
set size 0.8, 0.90
set grid
#set border 16

##? bind "ctrl-a"          # shows the binding for ctrl-a
   ##?bind 


set tmargin 2
set xtics rotate by -25                
set ylabel "Humdity" 
set autoscale y  
set ytics 0,.5,13


unset key


#set terminal pngcairo size 1200,600 enhanced font " font,4"
#set output 'output.png'


plot "../bin/data/history4.csv" using 1:3 with lines linetype 1 linewidth 1
 


#
