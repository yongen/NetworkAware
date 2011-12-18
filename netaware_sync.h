#ifndef NETAWARE_AYNC_H
#define NETAWARE_SYNC_H


#include "netaware.h"
#include "netaware_sync_distributed.h"

#include <mpi.h>

typedef struct netaware_sync_file
{
	//file
	MPI_File fh;

	int last_op;
	//first level buffer
	int bfptr;
	char * data;
	int bfstart, bfend;
	MPI_Offset offset;
	MPI_offset max_offset;
	//second level buffer
	struct netaware_sync_distributed_file * dist_buffer;
} * netaware_sync_fh;

netaware_sync_fh netaware_sync_fopen(char * filename, int mode);

int	netaware_sync_fwrite(netaware_sync_fh handle, void * buf, int count, MPI_Datatype type);

int netaware_sync_fseek(netaware_sync_fh handle, MPI_Offset offset, int whence);

int netaware_sync_flush(netaware_sync_fh handle);

int netaware_sync_fread(netaware_sync_fh handle,void *buf, int count, MPI_Datatype type);

int netaware_sync_fclose(netaware_sync_fh handle);

#endif
