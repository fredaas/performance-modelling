set terminal pdf enhanced size 3.5,3 font 'courier'
set size 1,1
set xtic rotate by 0 right offset 0.5,0
set key l L
set grid ytics
set style data lp

set output 'out_stream.pdf'

set offset graph 0, 0, 20, 0

set xlabel 'Threads'
set ylabel 'Bandwith [GB/s]'

set title 'IDUN: Intel Xeon E5-2630 v2'
plot 'idun_e5_scatter.txt' u 1:($3) title 'Scatter', \
     'idun_e5_compact.txt' u 1:($3) title 'Compact', \

set title 'IDUN: Intel Xeon Gold 6132'
plot 'idun_gold_scatter.txt' u 1:($3) title 'Scatter', \
     'idun_gold_compact.txt' u 1:($3) title 'Compact', \

set title 'FRAM: Intel Xeon E5-2683 v4'
plot 'fram_e5_scatter.txt' u 1:($3) title 'Scatter', \
     'fram_e5_compact.txt' u 1:($3) title 'Compact', \

set title 'BETZY'
plot 'betzy_epyc_compact.txt' u 1:($3) title '', \
