#01/09/2022 00:00:00, 66.8, 75.2,1014.5

set timefmt "%m/%d/%Y %H:%M:%S"
set datafile separator ","
set xdata time
set xrange [*:*]
<<<<<<< HEAD
=======
<<<<<<< HEAD

=======
>>>>>>> e05c1d0184e9193e4501d0ba00098dbade4f6c9d
>>>>>>> 63a8f604f8b078e67445c032818832b9e391d953
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
<<<<<<< HEAD

=======
<<<<<<< HEAD
set mytics 10        
set mxtics 8        
 
=======

>>>>>>> e05c1d0184e9193e4501d0ba00098dbade4f6c9d
>>>>>>> 63a8f604f8b078e67445c032818832b9e391d953
##? bind "ctrl-a"          # shows the binding for ctrl-a
   ##?bind 


set multiplot layout 1, 3 title "Multiplot layout 3, 1" font ",2"
set tmargin 2
set xtics rotate by -25                
<<<<<<< HEAD
set ylabel "Temp" 
=======
<<<<<<< HEAD
set ylabel "Temp F" 
=======
set ylabel "Temp" 
>>>>>>> e05c1d0184e9193e4501d0ba00098dbade4f6c9d
>>>>>>> 63a8f604f8b078e67445c032818832b9e391d953
set autoscale y  
unset key
plot "../bin/data/history4.csv" using 1:2 with lines linetype 1 linewidth 1
#
set xtics rotate by -25                
<<<<<<< HEAD
set ylabel "Humdity"
=======
<<<<<<< HEAD
set ylabel "Humdity %"
=======
set ylabel "Humdity"
>>>>>>> e05c1d0184e9193e4501d0ba00098dbade4f6c9d
>>>>>>> 63a8f604f8b078e67445c032818832b9e391d953

unset key
plot "../bin/data/history4.csv" using 1:3 with lines linetype 2 linewidth 2
#
#
set xtics rotate by -25                
<<<<<<< HEAD
set ylabel "Pres" 
=======
<<<<<<< HEAD
set ylabel "Pres " 
=======
set ylabel "Pres" 
>>>>>>> e05c1d0184e9193e4501d0ba00098dbade4f6c9d
>>>>>>> 63a8f604f8b078e67445c032818832b9e391d953

unset key
plot "../bin/data/history4.csv" using 1:4 with lines linetype 2 linewidth 2
#unset multiplot

