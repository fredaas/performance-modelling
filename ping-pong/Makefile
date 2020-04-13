.PHONY : clean purge plot

all : clean main

main : main.c
	$(CC) -o $@ $< $(CFLAGS)

plot :
	gnuplot plot.gp

clean :
	rm -f main

purge : clean
	rm -f *.txt *.png *.pdf
