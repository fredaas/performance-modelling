set terminal pdf enhanced size 4,3.5 font "courier"

set loadpath "./data"

unset key
set colorbox
set rmargin 8

set tic scale 0

set xtics 1 offset -0.5,0
set ytics 1
set border linewidth 0
set palette defined (0 "white", 1 "#424242")
set view map

set output "comms1_map.pdf"
set xlabel "Rank"
set ylabel "Rank"

set ytics 20
set xtics 20

set xrange [-0.5:127.5]
set yrange [-0.5:127.5]
set title "Fram-E5: Setup latency"
plot "fram_e5_alpha128.txt" matrix with image
set title "Fram-E5: Inverse bandwidth"
plot "fram_e5_beta128.txt" matrix with image

set xrange [-0.5:119.5]
set yrange [-0.5:119.5]
set title "Idun-E5: Setup latency"
plot "idun_e5_alpha120.txt" matrix with image
set title "Idun-E5: Inverse bandwidth"
plot "idun_e5_beta120.txt" matrix with image

set xrange [-0.5:111.5]
set yrange [-0.5:111.5]
set title "Idun-Gold: Setup latency"
plot "idun_gold_alpha112.txt" matrix with image
set title "Idun-Gold: Inverse bandwidth"
plot "idun_gold_beta112.txt" matrix with image
