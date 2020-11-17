all : clean stream params

params : params.c
	$(CC) -o $@ $< $(CFLAGS)

stream : stream.c
	$(CC) -o $@ $< $(CFLAGS)

clean :
	rm -rf stream params

purge : clean
	rm -rf *.txt *.pdf *.png
