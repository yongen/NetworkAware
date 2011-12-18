#include "netaware_native.h"

netaware_native_fh netaware_native_fopen(char * filename, int mode)
{
	int status;
	netaware_native_fh handle = malloc(sizeof(struct netaware_native_file));
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
	return handle;
}

int netaware_native_fwrite(netaware_native_fh handle, void * buf, int count,
		MPI_Datatype type)
{
	MPI_File_write(handle->fh, buf, count, MPI_BYTE, MPI_STATUS_IGNORE);
	return 0;
}

int netaware_native_fwrite_at(netaware_native_fh handle, MPI_Offset offset,
		void * buf, int count, MPI_Datatype type)
{
	MPI_File_write_at(handle->fh, offset, buf, count, MPI_BYTE,
			MPI_STATUS_IGNORE);
	return 0;
}

int netaware_native_fseek(netaware_native_fh handle, MPI_Offset offset,
		int whence)
{
	MPI_File_seek(handle->fh, offset, whence);
	return 0;
}

int netaware_native_fread(netaware_native_fh handle, void *buf, int count,
		MPI_Datatype type)
{
	MPI_Status status;
	int count;
	MPI_File_read(handle->fh, buf, count, MPI_BYTE, &status);
	MPI_Get_count(&status, MPI_BYTE, &count);
	return count;
}

int netaware_native_fread_at(netaware_native_fh handle, MPI_Offset offset,
		void *buf, int count, MPI_Datatype type)
{
	MPI_Status status;
	int count;
	MPI_File_read_at(handle->fh, offset, buf, count, MPI_BYTE, &status);
	MPI_Get_count(&status, MPI_BYTE, &count);
	return count;
}

int netaware_native_fclose(netaware_native_fh handle)
{
	MPI_File_close(&handle->fh);
	return 0;
}
