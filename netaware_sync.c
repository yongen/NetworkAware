#include "netware_sync.h"

netaware_sync_fh netaware_sync_fopen(char * filename, int mode)
{
	int status;
	netaware_sync_fh handle = malloc(sizeof(struct netaware_sync_file));
	handle->data = malloc(buffer_size);
	memset(handle->data, 0, buffer_size);
	handle->bfptr = 0;
	handle->bfsize = buffer_size;
	handle->bfstart = 0;
	handle->bfend = -1;
	handle->offset = 0.0f;
	handle->max_offset = 0.0f;
	handle->mode = mode;

	status = MPI_File_open(MPI_COMM_WORLD, filename, mode, MPI_INFO_NULL,
			&handle->fh);
	if (status != MPI_SUCCESS)
	{
		char error_string[MPI_MAX_ERROR_STRING];
		int length_of_error_string, error_class;
		MPI_Error_class(status, &error_class);
		MPI_Error_string(error_class, error_string, &length_of_error_string);
		fprintf(stderr, "%s\n", error_string);
		MPI_Error_string(status, error_string, &length_of_error_string);
		fprintf(stderr, "%s\n", error_string);
		MPI_Abort(MPI_COMM_WORLD, status);
	}
	handle->dist_buffer = netaware_sync_dist_fopen();
	handle->last_op = OPERATION_OPEN;
	return handle;
}

int netaware_sync_fwrite(netaware_sync_fh handle, void * buf, int count,
		MPI_Datatype type)
{
	int size;
	MPI_Aint type_size;
	MPI_Type_size(type, &type_size);
	size = count * type_size;
	if (OPERATION_READ == handle->last_op)
	{
		netaware_sync_flush(handle);
		handle->last_op = OPERATION_WRITE;
	}
	if (handle->bfptr + size < handle->bfsize)
	{
		memcpy(handle->data + handle->bfptr, buf, size);
		handle->bfptr += size;
	}
	else
	{
		part = handle->bfsize - handle->bfptr;
		remain = handle->bfptr + size - handle->bfsize;
		memcpy(handle->data + handle->bfptr, buf, part);
		handle->bfptr += part;
		//move data from local buffer to distributed buffer
		netaware_sync_dist_fwrite(handle);

		while (remain > local_handle->bfsize)
		{
			memcpy(handle->data, (char *) buf + part, handle->bfsize);
			//move data to distributed buffer
			handle->bfptr += handle->bfsize;
			netaware_sync_dist_fwrite(handle);

			remain -= handle->bfsize;
			part += handle->bfsize;
		}
		memcpy(handle->data, (char *) buf + part, remain);
		handle->bfptr += remain;
	}
	return 0;
}

int netaware_sync_fseek(netaware_sync_fh handle, MPI_Offset offset, int whence)
{
	if (whence == MPI_SEEK_CUR)
	{
		if (0 == offset)
		{
			return 0;
		}
		else
		{
			netaware_sync_dist_fwrite(handle);
			if (offset > 0)
			{
				int ptr = offset % buffer_size;
				handle->bfstart = ptr;
				handle->bfptr = ptr;
				handle->offset += offset - ptr;
			}
			else
			{
				int ptr = offset % buffer_size;
				handle->bfstart = buffer_size + ptr;
				handle->bfptr = buffer_size + ptr;
				handle->offset += offset - ptr - buffer_size;
			}
		}
	}
	else if (MPI_SEEK_SET == whence)
	{
		netaware_sync_dist_fwrite(handle);
		int ptr = offset % buffer_size;
		handle->bfstart = ptr;
		handle->bfptr = ptr;
		handle->offset = offset - ptr;
	}
	handle->last_op = OPERATION_SEEK;
	return 0;
}

int netaware_sync_flush(netaware_sync_fh handle)
{
	netaware_sync_dist_fwrite(handle);
	handle->bfstart = 0;
	handle->bfptr = 0;
	handle->bfend = -1;
	return 0;
}

int netaware_sync_fread(netaware_sync_fh handle, void *buf, int count,
		MPI_Datatype type)
{
	int size, remain, avail;
	MPI_Aint type_size;
	MPI_Type_size(type, &type_size);
	size = count * type_size;

	if (OPERATION_WRITE == handle->last_op)
	{
		netaware_sync_flush(handle);
		handle->last_op = OPERATION_READ;
	}

	if (handle->bfend == -1)
	{
		netaware_sync_dist_fread(handle);
	}
	/** read from buffer */
	char * curbuf;
	curbuf = (char *) buf;
	remain = size;
	while (remain > 0 && handle->bfend > 0 && handle->bfptr + remain
			>= handle->bfend)
	{
		avail = handle->bfend - handle->bfptr;
		memcpy(curbuf, (char *) handle->data + handle->bfptr, avail);
		curbuf += avail;
		remain -= avail;

		/* refill buffer */
		netaware_sync_dist_fread(handle);
		handle->bfptr = 0;
	}

	if (handle->bfend == 0)
	{
		/* ran out of data, eof */
		return -1;
	}

	memcpy(curbuf, (char *) handle->data + handle->bfptr, remain);
	handle->bfptr += remain;
	return 0;
}

int netaware_sync_fclose(netaware_sync_fh handle)
{
	netaware_sync_dist_close(handle);
	MPI_File_close(&handle->fh);
	if (handle->bfsize > 0)
	{
		free(handle->data);
	}
	free(handle);
	return 0;
}
