#01/09/2022 00:00:00, 66.8, 75.2,1014.5

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
#set output 'all.png'

set terminal wxt size 1200,600
show terminal

set xtics rotate
set multiplot
set tmargin 4
set bmargin  6
set rmargin 2
set lmargin 10
set size 0.8, 0.90
set grid
set mytics 10        
set mxtics 8        
 
##? bind "ctrl-a"          # shows the binding for ctrl-a
   ##?bind 


set multiplot layout 1, 3 title "Multiplot layout 3, 1" font ",2"
set tmargin 2
set xtics rotate by -25                
set ylabel "Temp F" 
set autoscale y  
unset key
plot "../bin/data/history4.csv" using 1:2 with lines linetype 1 linewidth 1
#
set xtics rotate by -25                
set ylabel "Humdity %"

unset key
plot "../bin/data/history4.csv" using 1:3 with lines linetype 2 linewidth 2
#
#
set xtics rotate by -25                
set ylabel "Pres " 

unset key
plot "../bin/data/history4.csv" using 1:4 with lines linetype 2 linewidth 2
#unset multiplot

