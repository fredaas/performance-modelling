
all : clean main

main : main.cu
	nvcc -o $@ $^ -std=c++11 -O2

clean :
	rm -f main
