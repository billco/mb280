
set terminal wxt size 1200,600


#set terminal pngcairo size 1200,600 enhanced font " font,4"
#set output 'output.png'

show terminal
 
set timefmt "%m/%d/%Y %H:%M:%S"
set datafile separator ","
set xdata time
set xtics time
set autoscale x  

set xrange [*:*]
#set xrange ['01/10/2022 05:00:00':'01/10/2022 00:00:00']
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
set grid linetype 8 linewidth 1


set tmargin 2
set xtics rotate by -25                
set ylabel "Temp F" 
set autoscale y  
#set tics 1
# not work set ytics 0,.5,10
#set mytics (60,.5,70)
#show tics
set mytics 5
set mxtics 8

unset key
#set mouse mouseformat "mouse = %a %H:%M:%S, %10.2f"
set mouse mouseformat 3
##? set mouse labeloptions 3

#plot "../bin/data/history4.csv" using 1:2 with lines linetype 1 linewidth 1
#"../bin/data/history1.csv" using 1:2 with lines linetype 2 linewidth 2
 
#while( 1 ) {
  plot "../bin/data/history4.csv" using 1:2 with lines linetype 1 linewidth 1 , \
#'../bin/data/history4h.csv' using 1: int(column(4)) & 0x0xf00c ? 70: 40  with lines linetype 2 linewidth 2

 #  pause 60 * 3 
  # }

## eof
