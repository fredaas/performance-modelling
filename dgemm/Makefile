all : clean dgemm

dgemm : dgemm.c
	$(CC) -o dgemm $< $(CFLAGS) /opt/OpenBLAS/lib/libopenblas.a -lpthread -lopenblas -DOPENBLAS

clean :
	rm -rf dgemm
