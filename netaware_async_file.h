#ifndef NETAWARE_ASYNC_FILE_H
#define NETAWARE_ASYNC_FILE_H

#include <mpi.h>

int netaware_async_file_fwrite(netaware_sync_fh * handle);

int netaware_async_file_fread(netaware_sync_fh * handle);

#endif
