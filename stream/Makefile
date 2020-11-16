all : clean stream

stream : stream.c
	$(CC) -o $@ $< $(CFLAGS)

clean :
	rm -rf stream

purge : clean
	rm -rf *.txt *.pdf *.png
