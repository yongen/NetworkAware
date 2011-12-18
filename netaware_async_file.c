
#include "netaware_async_file.h"

int netaware_async_file_fwrite(netaware_sync_fh * handle)
{
	MPI_Offset offset_tmp;
	netaware_sync_distributed_fh dist_handle = handle->dist_buffer;
	MPI_Win_fence(0, dist_handle->win);
	MPI_Offset offset = handle->rank * buffer_size;
	int i = 0;
	//broadcast to find the max offset
	MPI_Allreduce(&handle->max_offset, &offset_tmp, 1, MPI_Offset, MPI_MAX,
			MPI_COMM_WORLD);
	handle->max_offset = offset_tmp;

	while (offset <= handle->max_offset)
	{
		int count = MIN(handle->max_offset - offset, buffer_size);
		MPI_File_write_at(handle->fh, offset,
				dist_handle->data[i * buffer_size], count, MPI_BYTE,
				MPI_STATUS_IGNORE);
		i++;
		offset += buffer_size * handle->dist_handle->num_procs;
	}
	return 0;
}

int netaware_async_file_fread(netaware_sync_fh * handle)
{
	MPI_Status status;
	MPI_Offset offset_tmp;
	int count = 0;
	netaware_async_distributed_fh dist_handle = handle->dist_buffer;
	MPI_Win_fence(0, dist_handle->win);

	MPI_Offset offset = handle->rank * buffer_size;
	MPI_File_seek(handle->fh, offset, MPI_SEEK_CUR);
	MPI_File_read(handle->fh, dist_handle->data[0], handle->bfsize, MPI_BYTE,
			&status);
	MPI_Get_count(&status, MPI_BYTE, &count);
	int i = 1;
	while (count == buffer_size)
	{
		MPI_File_read(handle->fh, dist_handle->data[i], handle->bfsize,
				MPI_BYTE, &status);
		MPI_Get_count(&status, MPI_BYTE, &count);
		i++;
		MPI_File_seek(handle->fh, buffer_size * dist_handle->num_procs,
				MPI_SEEK_CUR);
	}
	offset_tmp = (i - 1) * dist_handle->num_procs * buffer_size + count;
	//broadcast to find the max offset
	MPI_Allreduce(&offset_tmp, &handle->max_offset, 1, MPI_Offset, MPI_MAX,
			MPI_COMM_WORLD);
	return 0;
}
