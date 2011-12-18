#ifndef NETAWARE_ASYNC_DISTRIBUTED_H
#define NETAWARE_ASYNC_DISTRIBUTED_H

#include "netaware_async.h"
#include "netaware_async_file.h"

typedef struct netaware_async_distributed_file
{
	int bfsize;
	int bfpages;
	char * data;
	int num_procs;
	int rank;
	MPI_Win win;
}* netaware_async_distributed_fh;

netaware_async_distributed_fh netaware_async_dist_fopen();

int netaware_async_dist_fwrite(netaware_async_fh * handle);

int netaware_async_dist_flush(netaware_async_fh * handle);

int netaware_async_dist_fetch(netaware_async_fh * handle);

int netaware_async_dist_fread(netaware_async_fh * handle);

int netaware_async_dist_close(netaware_async_fh * handle);


#endif
