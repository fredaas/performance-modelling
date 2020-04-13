set terminal pdf enhanced size 4,3.5 font "courier"

unset key
set colorbox
set rmargin 8

set tic scale 0
set xrange [-0.5:3.5]
set yrange [-0.5:3.5]
set xtics 1 offset -0.5,0
set ytics 1
set border linewidth 0
set palette defined (0 "white", 1 "red")
set view map

set output "out.pdf"
set xlabel "Rank"
set ylabel "Rank"

# set cblabel "Latency" offset 2,0
set title "Ping-pong: Latency"
plot "out_alpha.txt" matrix with image

# set cblabel "Inverse Bandwidth" offset 2,0
set title "Ping-pong: Inverse Bandwidth"
plot "out_beta.txt" matrix with image
