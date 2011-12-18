int netaware_delay_meta_fwrite(netaware_delay_fh handle)
{
	netaware_delay_distributed_buffer dist_handle = handle->dist_buffer;
	meta_for_merge meta_data = dist_handle->meta_data;

	MPI_Datatype meta_type = get_meta_type();
	MPI_Datatype array_type;
	MPI_Type_contiguous(meta_data->count, eType, &array_type);
	if (0 == dist_handle->rank)
	{
		MPI_File_write(handle, dist_handle->total_size, 1, MPI_INT, &status);
	}
	int type_size;
	MPI_Type_size(MPI_INT, &type_size);
	MPI_File_set_view(dist_handle->fh, dist_handle->offset + type_size,
			meta_type, array_type, "native", MPI_INFO_NULL);
	MPI_File_write(dist_handle->fh, meta_data->array, meta_data->count,
			meta_type, &status);

	return 0;
}

int netaware_delay_file_fwrite(netaware_delay_fh handle)
{
	MPI_File_write(handle->fh, handle->data, handle->bfptr, MPI_BYTE,
			MPI_STATUS_IGNORE);
	return 0;
}

int netaware_delay_meta_fread(netaware_delay_fh handle)
{
	//to do
	netaware_delay_distributed_buffer dist_handle = handle->dist_buffer;
	MPI_Offset * offsets;
	int * recvcnts;
	int send_count;
	int recv_account = 1;
	if (0 == dist_handle->rank)
	{
		MPI_File_read(dist_handle->fh, dist_handle->total_size, 1, MPI_INT,
				&status);
		send_count = dist_handle->total_size;
		recvcnts = malloc(sizeof(int) * dist_handle->num_procs);

		int offset_size;
		MPI_Type_size(MPI_Offset, &offset_size);

		offsets = malloc(offset_size * dist_handle->num_procs);
		int remain_num_procs = dist_handle->num_procs;
		int remain_num_meta = total_size;
		int meta_size = sizeof(struct meta_for_mpi_struct);

		for (int i = 0; i < dist_handle->num_procs; i++)
		{
			recvcnts[i]
					= remain_num_meta % remain_num_procs == 0 ? remain_num_meta
							/ remain_num_procs : remain_num_meta
							/ remain_num_procs + 1;
			remain_num_meta -= recvcnts[i];
			remain_num_procs--;
			if (0 == i)
			{
				offset[i] = 0;
			}
			else
			{
				offsets[i] = offset[i - 1] + recvcnts[i] * meta_size;
			}
		}
	}
	int meta_data_count;
	MPI_Scatter(recvcnts, send_count, MPI_INT, &meta_data_count, recv_count,
			MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(offsets, send_count, MPI_Offset, &dist_handle->offset,
			recv_count, MPI_Offset, 0, MPI_COMM_WORLD);

	meta_for_merge meta_data = malloc(sizeof(struct meta_for_merge_struct));
	meta_data->count = meta_data_count;
	meta_data->array = malloc(sizeof(struct meta_for_mpi_struct)*meta_data_count);
	MPI_Datatype meta_type = get_meta_type();
	MPI_Datatype array_type;
	MPI_Type_contiguous(meta_data->count, meta_type, &array_type);
	int int_size;
	MPI_Type_size(MPI_INT, &int_size);
	MPI_File_set_view(dist_handle->fh, int_size + dist_handle->offset, meta_type, array_type,
			"native", MPI_INFO_NULL);

	MPI_File_read(dist_handle->fh, meta_data->array, meta_data->count, meta_type, &status);
	dist_handle->meta_data = meta_data;
	return 0;
}

int netaware_delay_file_fread(netaware_delay_fh handle)
{
	int count;
	MPI_Status status;
	MPI_File_read(handle->fh, handle->data, handle->bfsize, MPI_BYTE,
				&status);
	MPI_Get_count(&status, MPI_BYTE, &count);
	return count;
}
