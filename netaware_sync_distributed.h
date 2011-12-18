#ifndef NETAWARE_DISTRIBUTED_H
#define NETAWARE_DISTRIBUTED_H
#include "netaware.h"
#include "netaware_sync.h"
#include "netaware_sync_file.h"

#define MAX(a,b)	((a>b)?(a):(b))
#define MIN(a,b)	((a<b)?(a):(b))

typedef struct netaware_sync_distributed_file
{
	//buffer size of second level
	int bfsize;
	//second level buffer
	int bfpages;
//	int * id;
	char *data;
	int num_procs;
	int rank;
	MPI_Win win;
}* netaware_sync_distributed_fh;

netaware_sync_distributed_fh netaware_sync_dist_fopen();

int netaware_sync_dist_fwrite(netaware_sync_fh * handle);

int netaware_sync_dist_flush(netaware_sync_fh * handle);

int netaware_sync_dist_fread(netaware_sync_fh * handle);

int netaware_sync_dist_close(netaware_sync_fh * handle);

#endif
