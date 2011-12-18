#include "netaware.h"

netaware_fh netaware_fopen(char * filename, int mode)
{
	netaware_fh handle = malloc(sizeof(struct netaware_file));

	//initial first level buffer
	if (NETWORK_SYNC == network_strategy)
	{
		handle->sync = netaware_sync_fopen(filename, mode);
	}
	else if (NETWORK_ASYNC == network_strategy)
	{
		//to do
		handle->async = netaware_async_fopen(filename, mode);
	}
	else if (NETWORK_DELAY == network_strategy)
	{
		//to do
		handle->delay = netaware_delay_fopen(filename, mode);
	}
	else if (NETWORK_NATIVE == network_strategy)
	{
		//to do
		handle->native = netaware_delay_fopen(filename, mode);
	}
	return handle;
}

int netaware_fwrite(netaware_fh handle, void * buf, int count,
		MPI_Datatype type)
{
	if (NETWORK_SYNC == network_strategy)
	{
		netaware_sync_fwrite(handle->sync, buf, count, type);
	}
	else if (NETWORK_ASYNC == network_strategy)
	{
		//to do
		netaware_async_fwrite(handle->async, buf, count, type);
	}
	else if (NETWORK_DELAY == network_strategy)
	{
		//to do
		netaware_delay_fwrite(handle->delay, buf, count, type);
	}
	else if (NETWORK_NATIVE == network_strategy)
	{
		//to do
		netaware_native_fwrite(handle->native, buf, count, type);
	}
	return 0;
}

int netaware_fseek(netaware_fh handle, MPI_Offset offset, int whence)
{
	if (NETWORK_SYNC == network_strategy)
	{
		netaware_sync_fseek(handle->sync, offset, whence);
	}
	else if (NETWORK_ASYNC == network_strategy)
	{
		//to do
		netaware_async_fseek(handle->async, offset, whence);
	}
	else if (NETWORK_DELAY == network_strategy)
	{
		//to do
		netaware_delay_fseek(handle->delay, offset, whence);
	}
	else if (NETWORK_NATIVE == network_strategy)
	{
		//to do
		netaware_native_fseek(handle->native, offset, whence);
	}
	return 0;
}

int netaware_fwrite_at(netaware_fh handle, MPI_Offset offset, void * buf,
		int count, MPI_Datatype type)
{
	if (NETWORK_SYNC == network_strategy)
	{
		netaware_sync_fseek(handle->sync, offset, whence);
		netaware_sync_fwrite(handle->sync, buf, count, type);
	}
	else if (NETWORK_ASYNC == network_strategy)
	{
		//to do
		netaware_async_fseek(handle->async, offset, whence);
		netaware_async_fwrite(handle->async, buf, count, type);
	}
	else if (NETWORK_DELAY == network_strategy)
	{
		//to do
		netaware_delay_fwrite_at(handle->delay,offset, buf, count, type);
	}
	else if (NETWORK_NATIVE == network_strategy)
	{
		//to do
		netaware_native_fseek(handle->native, offset, whence);
		netaware_native_fwrite(handle->native, buf, count, type);
	}

	return 0;
}

int netaware_flush(netaware_fh handle)
{
	if (NETWORK_SYNC == network_strategy)
	{
		netaware_sync_flush(handle->sync);
	}
	else if (NETWORK_ASYNC == network_strategy)
	{
		//to do
		netaware_async_flush(handle->async);
	}
	else if (NETWORK_DELAY == network_strategy)
	{
		//to do
		netaware_delay_flush(handle->delay);
	}
	else if (NETWORK_NATIVE == network_strategy)
	{
		//to do
		netaware_native_flush(handle->native);
	}

	return 0;
}

int netaware_fread(netaware_fh handle, void *buf, int count, MPI_Datatype type)
{
	//
	if (NETWORK_SYNC == network_strategy)
	{
		netaware_sync_fread(handle->sync, buf, count, type);
	}
	else if (NETWORK_ASYNC == network_strategy)
	{
		//to do
		netaware_async_fread(handle->async, buf, count, type);
	}
	else if (NETWORK_DELAY == network_strategy)
	{
		//to do
		netaware_delay_fread(handle->delay, buf, count, type);
	}
	else if (NETWORK_NATIVE == network_strategy)
	{
		//to do
		netaware_native_fread(handle->native, buf, count, type);
	}
	return 0;
}

int netaware_fread_at(netaware_fh handle, MPI_Offset offset, void *buf,
		int count, MPI_Datatype type)
{
	if (NETWORK_SYNC == network_strategy)
	{
		netaware_sync_fseek(handle->sync, offset, whence);
		netaware_sync_fread(handle->sync, buf, count, type);
	}
	else if (NETWORK_ASYNC == network_strategy)
	{
		//to do
		netaware_async_fseek(handle->async, offset, whence);
		netaware_async_fread(handle->async, buf, count, type);
	}
	else if (NETWORK_DELAY == network_strategy)
	{
		//to do
		netaware_delay_fseek(handle->delay, offset, whence);
		netaware_delay_fread(handle->delay, buf, count, type);
	}
	else if (NETWORK_NATIVE == network_strategy)
	{
		//to do
		netaware_native_fread_at(handle->native,offset, buf, count, type);
	}
	return 0;
}

int netaware_fclose(netaware_fh handle)
{
	if (NETWORK_SYNC == network_strategy)
	{
		netaware_sync_fclose(handle->sync);
	}
	else if (NETWORK_ASYNC == network_strategy)
	{
		//to do
		netaware_async_fclose(handle->async);
	}
	else if (NETWORK_DELAY == network_strategy)
	{
		//to do
		netaware_delay_fclose(handle->delay);
	}
	else if (NETWORK_NATIVE == network_strategy)
	{
		//to do
		netaware_native_fclose(handle->native);
	}
	return 0;
}

