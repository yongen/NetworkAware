#ifndef NETAWARE_NATIVE_H
#define NETAWARE_NATIVE_H

#include <mpi.h>

typedef struct netaware_native_file
{
	MPI_File fh;
}* netaware_native_fh;

netaware_native_fh netaware_native_fopen(char * filename, int mode);

int netaware_native_fwrite(netaware_native_fh handle, void * buf, int count,
		MPI_Datatype type);

int netaware_native_fwrite_at(netaware_native_fh handle, MPI_Offset offset, void * buf, int count,
		MPI_Datatype type);

int netaware_native_fseek(netaware_native_fh handle, MPI_Offset offset,
		int whence);

int netaware_native_fread(netaware_native_fh handle, void *buf, int count,
		MPI_Datatype type);

int netaware_native_fread_at(netaware_native_fh handle, MPI_Offset offset, void *buf, int count,
		MPI_Datatype type);

int netaware_native_fclose(netaware_native_fh handle);

#endif
