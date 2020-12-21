set terminal pdf enhanced size 3,3 font 'courier'
set size 1,1
set xtic rotate by 0 right offset 0.5,0
set key r
set grid ytics
set style data lp

set ylabel 'Bandwith [GB/s]'
set xlabel 'Message size'
set output 'comms1_line.pdf'

set xtics 100000
set xrange[10000:]

# set title 'Fram-E5: Intra-socket bandwidth'
# plot 'fram_intra-socket.txt'    u 1:2 notitle, \

# set title 'Fram-E5: Inter-socket bandwidth'
# plot 'fram_inter-socket.txt'    u 1:2 notitle, \

# set title 'Fram-E5: Inter-node bandwidth'
# plot 'fram_inter-node.txt'    u 1:2 notitle, \
