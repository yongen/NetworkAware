#include "netaware_delay.h"
#include "netaware_delay_distributed.h"

netaware_delay_fh netaware_delay_fopen(char * filename, int mode)
{
	int status, rank;
	char filename_str[256];
	struct netaware_delay_file * handle = malloc(
			sizeof(struct netaware_delay_file));
	handle->data = malloc(buffer_size);
	memset(handle->data, 0, buffer_size);
	handle->bfptr = 0;
	handle->bfsize = buffer_size;
	handle->bfstart = 0;
	handle->bfend = -1;
	handle->offset = 0.0f;
	handle->max_offset = 0.0f;
	handle->mode = mode;

	handle->meta_data = init_meta_queue();

	MP_Comm_rank(MPI_COMM_WORLD, &rank);
	sprintf(filename_str, "%s_%d.data", filename, rank);
	status = MPI_File_open(MPI_COMM_WORLD, filename_str, mode, MPI_INFO_NULL,
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
	handle->dist_buffer = netaware_delay_dist_fopen();

	return handle;
}

int netaware_delay_fwrite(netaware_delay_fh handle, void * buf, int count,
		MPI_Datatype type)
{
	int size;
	MPI_Aint type_size;
	MPI_Type_size(type, &type_size);
	size = count * type_size;
	if (handle->bfptr + size < handle->bfsize)
	{
		memcpy(handle->data + handle->bfptr, buf, size);
		handle->bfptr += size;
		handle->length += size;
	}
	else
	{
		part = handle->bfsize - handle->bfptr;
		remain = handle->bfptr + size - handle->bfsize;
		memcpy(handle->data + handle->bfptr, buf, part);
		handle->bfptr += part;
		handle->length += part;

		//move data from local buffer to distributed buffer
		netaware_delay_file_fwrite(handle);

		while (remain > local_handle->bfsize)
		{
			memcpy(handle->data, (char *) buf + part, handle->bfsize);
			handle->bfptr += handle->bfsize;
			handle->length += handle->bfsize;

			//move data to distributed buffer
			netaware_delay_file_fwrite(handle);

			remain -= handle->bfsize;
			part += handle->bfsize;
		}
		memcpy(handle->data, (char *) buf + part, remain);
		handle->bfptr += remain;
		handle->length += remain;
	}
	return 0;
}

int netaware_delay_meta_fwrite()
{
	meta = malloc(sizeof(struct meta_struct));
	meta->offset = handle->offset;
	meta->length = handle->length;
	insert_queue(handle->meta_queue, meta);
	return 0;
}

int netaware_delay_fseek(netaware_delay_fh handle, MPI_Offset offset,
		int whence)
{
	if (whence == MPI_SEEK_CUR)
	{
		if (0 == offset)
		{
			return 0;
		}
		else
		{
			netaware_delay_meta_fwrite();
			handle->offset += offset;
			handle->length = 0;
		}
	}
	else if (MPI_SEEK_SET == whence)
	{
		netaware_delay_meta_fwrite();
		handle->offset = offset;
		handle->length = 0;
	}
	return 0;
}

int netaware_delay_flush(netaware_delay_fh handle)
{
	//meta data flush
	metaware_delay_dist_fwrite(handle);

	// data file flush
	netaware_delay_file_fwrite(handle);
	handle->bfptr = 0;
	handle->length = 0;
	handle->bfend = -1;

	return 0;
}

int netaware_delay_fread(netaware_delay_fh handle, void *buf, int count,
		MPI_Datatype type)
{
	int size, remain, avail;
	MPI_Aint type_size;
	MPI_Type_size(type, &type_size);
	size = count * type_size;
	if (handle->bfend == -1)
	{
		netaware_delay_meta_fread(handle);
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

int netaware_delay_fclose(netaware_delay_fh handle)
{

}
