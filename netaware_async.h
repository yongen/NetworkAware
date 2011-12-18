#ifndef NETAWARE_ASYNC_H
#define NETAWARE_ASYNC_H

#include "netaware.h"
#include "netaware_async_distributed.h"
#include "netaware_async_segment.h"


typedef struct netaware_async_file
{
	//file
	MPI_File fh;

	int last_op;
	//first level buffer
	int bfptr;
	char * data;
	int bfstart, bfend;

	struct segment_queue_struct * segment_queue;

	MPI_Offset offset;
	MPI_offset max_offset;
	//second level buffer
	struct netaware_async_distributed_buffer * dist_buffer;
}* netaware_async_fh;


netaware_async_fh netaware_async_fopen(char * filename, int mode);

int netaware_async_fwrite(netaware_async_fh handle, void * buf, int count,
		MPI_Datatype type);

int netaware_async_fseek(netaware_async_fh handle, MPI_Offset offset,
		int whence);

int netaware_async_flush(netaware_async_fh handle);

int netaware_async_fetch(netaware_async_fh handle);

int netaware_async_fread(netaware_async_fh handle, void *buf, int count,
		MPI_Datatype type);

int netaware_async_fclose(netaware_async_fh handle);

#endif
