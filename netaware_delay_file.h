#ifndef NETAWARE_DELAY_FILE_H
#define NETAWARE_DELAY_FILE_H

#include <mpi.h>

#include "netaware_delay.h"
#include "netaware_delay_meta.h"
#include "netaware_delay_distributed.h"


int netaware_delay_meta_fwrite(netaware_delay_fh handle);

int netaware_delay_file_fwrite(netaware_delay_fh handle);

int netaware_delay_meta_fread(netaware_delay_fh handle);

int netaware_delay_file_fread(netaware_delay_fh handle);


#endif
