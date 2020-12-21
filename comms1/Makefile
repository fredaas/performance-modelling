all : clean comms1_map comms1_saturate

comms1_map : comms1_map.c
	$(CC) -o $@ $< $(CFLAGS)

comms1_saturate : comms1_saturate.c
	$(CC) -o $@ $< $(CFLAGS)

clean :
	rm -f comms1_map comms1_saturate
