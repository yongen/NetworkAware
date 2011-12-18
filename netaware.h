#ifndef NETAWARE_H
#define NETAWARE_H

#include <mpi.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "netaware_sync.h"
#include "netaware_async.h"
#include "netaware_delay.h"
#include "netaware_native.h"

#define NETWORK_SYNC	0
#define NETWORK_ASYNC	1
#define NETWORK_DELAY	2
#define NETWORK_NATIVE	3

#define OPERATION_OPEN		0
#define	OPERATION_WRITE		1
#define OPERATION_READ		2
#define OPERATION_SEEK		3



typedef struct netaware_file
{
	//network strategy
	int net_tag;
	int mode;

	struct netaware_sync * sync;
	struct netaware_async * async;
	struct netaware_delay * delay;
	struct netaware_native * native;
}* netaware_fh;

netaware_fh netaware_fopen(char * filename, int mode);

int netaware_fwrite(netaware_fh handle, void * buf, int count,
		MPI_Datatype type);

int netaware_fseek(netaware_fh handle, MPI_Offset offset, int whence);

int netaware_fwrite_at(netaware_fh handle, MPI_Offset offset, void * buf,
		int count, MPI_Datatype type);

int netaware_flush(netaware_fh handle);

int netaware_fread(netaware_fh handle, void *buf, int count, MPI_Datatype type);

int netaware_fread_at(netaware_fh handle, MPI_Offset offset, void *buf,
		int count, MPI_Datatype type);

int netaware_fclose(netaware_fh handle);

#endif
