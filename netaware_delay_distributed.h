#ifndef NETAWARE_DELAY_DISTRIBUTED_H
#define NETAWARE_DELAY_DISTRIBUTED_H

#include "netaware_delay.h"
#include "netaware_delay_meta.h"

typedef struct netaware_delay_distributed_buffer
{
	int num_procs;
	int rank;
	int total_size;
	struct meta_for_merge_struct * meta_data;
	MPI_File fh;
	MPI_Offset offset;
}* distributed_delay_meta_fh;

distributed_delay_meta_fh netaware_delay_dist_fopen();

int netaware_delay_dist_fwrite(netaware_delay_fh * handle);

int netaware_delay_dist_flush(netaware_delay_fh * handle);

int netaware_delay_dist_fetch(netaware_delay_fh * handle);

int netaware_delay_dist_fread(netaware_delay_fh * handle);

int netaware_delay_dist_close(netaware_delay_fh * handle);

#endif
