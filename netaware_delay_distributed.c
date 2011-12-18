#include "netaware_delay_distributed.h"

distributed_delay_meta_fh netaware_delay_dist_fopen()
{
	char filename_str[256];
	distributed_delay_meta_fh dist_handle = malloc(
			sizeof(struct netaware_delay_distributed_buffer));

	MPI_Comm_size(MPI_COMM_WORLD, &dist_handle->num_procs);
	MP_Comm_rank(MPI_COMM_WORLD, &dist_handle->rank);
	dist_handle->meta_data = malloc(sizeof(struct meta_for_merge_struct));
	dist_handle->meta_data->count = 0;

	sprintf(filename_str, "%s.meta", filename);
	status = MPI_File_open(MPI_COMM_WORLD, filename_str, mode, MPI_INFO_NULL,
			&dist_handle->fh);
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

	return dist_handle;
}

int netaware_delay_dist_fwrite(netaware_delay_fh * handle)
{
	netaware_delay_distributed_buffer dist_handle = handle->dist_buffer;
	meta_queue meta_list = handle->meta_data;
	int total_size;
	MPI_Allreduce(&meta_list->size, &total_size, 1, MPI_INT, MPI_SUM,
			MPI_COMM_WORLD);
	dist_handle->total_size = total_size;
	meta_for_merge meta_array = get_meta_for_merge_array(meta_list);

	// merge new local meta data items with existing distributed meta data items
	meta_for_merge_function(&(dist_handle->meta_data), &meta_array, 0, NULL);

	//parallel merge all the local meta data
	MPI_Datatype meta_for_merge_type = get_meta_for_merge_type(meta_list);
	MPI_Op merge_op;
	MPI_Op_create(meta_for_merge_function, True, &merge_op);

	int meta_size = sizeof(struct meta_for_mpi_struct);
	MPI_Offset offset = 0;
	int * recvcnts = malloc(sizeof(int) * dist_handle->num_procs);
	int remain_num_procs = dist_handle->num_procs;
	int remain_num_meta = total_size;
	for (int i = 0; i < dist_handle->num_procs; i++)
	{
		recvcnts[i] = remain_num_meta % remain_num_procs == 0 ? remain_num_meta
				/ remain_num_procs : remain_num_meta / remain_num_procs + 1;
		remain_num_meta -= recvcnts[i];
		remain_num_procs--;
		if (dist_handle->rank == i)
		{
			dist_handle->offset = offset;
		}
		offset += recvcnts[i] * meta_size;
	}

	meta_for_merge meta_for_merge_recv = malloc(sizeof(meta_for_merge_struct)
			* recvcnts[dist_handle->rank]);

	MPI_Reduce_scatter(&meta_array, &meta_for_merge_recv, &recvcnts,
			meta_for_merge_type, merge_op, MPI_COMM_WORLD);

	//recycle memory
	free(meta_array->array);
	free(meta_array);
	free(dist_handle->meta_data->array);
	free(dist_handle->meta_data);

	dist_handle->meta_data = meta_for_merge_recv;
	return 0;
}

int netaware_delay_dist_flush(netaware_delay_fh * handle)
{
	netaware_delay_meta_fwrite(handle);
}

int netaware_delay_dist_fread(netaware_delay_fh * handle);

int netaware_delay_dist_close(netaware_delay_fh * handle);

