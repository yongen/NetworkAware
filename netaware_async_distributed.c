#include "netaware_async_distributed.h"


netaware_async_distributed_fh netaware_async_dist_fopen()
{
	netaware_async_distributed_fh dist_handle = malloc(
			sizeof(struct netaware_async_distributed_file));
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

int netaware_async_dist_fwrite(netaware_async_fh * handle)
{
	netaware_async_distributed_fh dist_handle = handle->dist_buffer;
	segment item = handle->segment_queue->head;
	while (NULL != item)
	{
		MPI_Offset offset = item->offset;
		handle->max_offset = MAX(handle->max_offset, offset);
		int rank_id = (offset / buffer_size) % dist_handle->num_procs;
		int block_id = (offset / buffer_size) / dist_handle->num_procs;
		int ptr = offset % buffer_size;
		MPI_Put(item->data, item->count, MPI_BYTE, rank_id, block_id
				* buffer_size + ptr, item->count, MPI_BYTE, dist_handle->win);
		segment tmp = item;
		item = item->next;
		free(tmp->data);
		free(tmp);
	}
	return 0;
}


int netaware_async_dist_flush(netaware_async_fh * handle)
{
	netaware_async_file_fwrite(handle);

	//	MPI_Win_fence(0, dist_handle->win);
	return 0;
}

int netaware_async_dist_fread(netaware_async_fh * handle)
{
	netaware_async_distributed_fh dist_handle = handle->dist_buffer;
	segment item = handle->segment_queue->head;
	if (handle->bfend == -1)
	{
		netaware_async_file_fread(handle);
	}
	while (NULL != item)
	{
		MPI_Offset offset = item->offset;
		int rank_id = (offset / buffer_size) % dist_handle->num_procs;
		int block_id = (offset / buffer_size) / dist_handle->num_procs;
		int ptr = offset % buffer_size;
		MPI_Get(item->data, item->count, MPI_BYTE, rank_id, block_id
				* buffer_size + ptr, item->count, MPI_BYTE, dist_handle->win);
		segment tmp = item;
		item = item->next;
		free(tmp);
	}
	return 0;
}

int netaware_async_dist_close(netaware_async_fh * handle)
{
	netaware_async_dist_flush(handle);
	netaware_async_distributed_fh dist_handle = handle->dist_buffer;
	MPI_Free_mem(dist_handle->data);
	MPI_Win_free(&handle->win);
	free(dist_handle);
}
