#include "netaware_sync_distributed.h"

netaware_sync_distributed_fh netaware_sync_dist_fopen()
{
	netaware_sync_distributed_fh dist_handle = malloc(
			sizeof(struct netaware_sync_distributed_file));
	dist_handle->bfpages = num_buffer_pages;
	dist_handle->bfsize = buffer_size;

	MPI_Alloc_mem(sizeof(char) * buffer_size * num_buffer_pages, MPI_INFO_NULL,
			&(dist_handle->data));
	MPI_Win_create(dist_handle->data, buffer_size * num_buffer_pages, 1,
			MPI_INFO_NULL, MPI_COMM_WORLD, &(dist_handle->win));
	MPI_Win_fence(0, dist_handle->win);
	MPI_Comm_size(MPI_COMM_WORLD, &dist_handle->num_procs);
	MP_Comm_rank(MPI_COMM_WORLD, &dist_handle->rank);
	return dist_handle;
}

int netaware_sync_dist_fwrite(netaware_sync_fh * handle)
{
	netaware_sync_distributed_fh dist_handle = handle->dist_buffer;
	handle->max_offset
			= MAX(handle->max_offset, handle->offset + handle->bfptr);
	int rank_id = (handle->offset / buffer_size) % dist_handle->num_procs;
	int count = handle->bfptr - handle->bfstart;
	int block_id = (offset / buffer_size) / dist_handle->num_procs;
	MPI_Put(handle->data + handle->bfstart, count, MPI_BYTE, rank_id, block_id
			* buffer_size + handle->bfstart, count, MPI_BYTE, dist_handle->win);
	handle->bfstart = 0;
	handle->bfptr = 0;
	handle->offset += handle->bfsize;
	return 0;
}

int netaware_sync_dist_flush(netaware_sync_fh * handle)
{
	netaware_sync_file_fwrite(handle);
	MPI_Win_fence(0, dist_handle->win);
	return 0;
}


int netaware_sync_dist_fread(netaware_sync_fh * handle)
{
	netaware_sync_distributed_fh dist_handle = handle->dist_buffer;
	// a special function is requried for load data and the max offset

	int rank_id = (handle->offset / buffer_size) % dist_handle->num_procs;
	int count = MIN(buffer_size, handle->max_offset - handle->offset);
	int block_id = (offset / buffer_size) / dist_handle->num_procs;
	MPI_Get(handle->data, count, MPI_BYTE, rank_id, block_id * buffer_size,
			count, MPI_BYTE, dist_handle->win);
	handle->bfend = count;
	handle->offset += handle->bfsize;
	return 0;
}


int netaware_sync_dist_close(netaware_sync_fh * handle)
{
	netaware_sync_dist_flush(handle);
	netaware_sync_distributed_fh dist_handle = handle->dist_buffer;
	MPI_Free_mem(dist_handle->data);
	MPI_Win_free(&handle->win);
	free(dist_handle);
}
