all : clean stream params

params : params.c
	$(CC) -o $@ $< -std=c99 -Wall -O2 -I./

stream : stream.c
	$(CC) -o $@ $< $(CFLAGS)

clean :
	rm -rf stream params

purge : clean
	rm -rf *.txt *.pdf *.png
