compile:	test

OBJS=	netaware.o netaware_sync.o netaware_sync_distributed.o netaware_sync_file.o \
		netaware_async.o netaware_async_segment.o netaware_async_distributed.o netaware_async_file.o \
		netaware_delay.o netaware_delay_meta.o netaware_delay_distributed.o netaware_delay_file.o

.c.o:
	mpicc -c $< -o $@

test:	$(OBJS)
	mpicc -lm -o $@ $? netawar_test.c

clean:
	rm *.o test
	
