#ifndef NETAWARE_DELAY_H
#define NETAWARE_DELAY_H

#include "netaware.h"
#include "netaware_delay_meta.h"

typedef struct netaware_delay_file
{
	MPI_File fh;
	int bfptr;
	char * data;
	int bfstart, bfend;

	struct meta_queue_struct * meta_data;

	MPI_Offset offset;
	MPI_Offset max_offset;
	int length;

	struct netaware_delay_distributed_buffer * dist_buffer;
} * netaware_delay_fh;

netaware_delay_fh netaware_delay_fopen(char * filename, int mode);

int netaware_delay_fwrite(netaware_delay_fh handle, void * buf, int count,
		MPI_Datatype type);

int netaware_delay_fseek(netaware_delay_fh handle, MPI_Offset offset,
		int whence);

int netaware_delay_flush(netaware_delay_fh handle);

int netaware_delay_fetch(netaware_delay_fh handle);

int netaware_delay_fread(netaware_delay_fh handle, void *buf, int count,
		MPI_Datatype type);

int netaware_delay_fclose(netaware_delay_fh handle);

#endif
