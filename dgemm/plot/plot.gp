set terminal pdf enhanced size 3.5,3 font 'courier'
set size 1,1
set xtic rotate by 0 right offset 0.5,0
set grid ytics
set style data lp

set yrange[0:]

set output 'dgemm.pdf'

set offset graph 0, 0, 20, 0

set xlabel 'Threads'
set ylabel 'FLOPS'

set title 'FRAM: Intel Xeon E5-2683 v4 '
plot 'fram_e5.txt' u 1:3  notitle, \

set title 'IDUN: Intel Xeon E5-2630 v2'
plot 'idun_e5.txt' u 1:3  notitle, \

set title 'IDUN: Intel Xeon Gold 6132'
plot 'idun_gold.txt' u 1:3  notitle, \
