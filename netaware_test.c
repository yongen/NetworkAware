#include "netaware.h"

//begin global setting
int network_strategy = 0;
int buffer_size = 1 << 20;
int num_buffer_pages = 5;
//end global setting

#define MAX_LEN	16

String fileName = "test";

int my_rank;
int num_procs;

double cols[MAX_LEN];

netaware_fh handle;

int init_cols()
{
	int i, j, k;
	k = 0;
	for (i = 0; i < MATRIX_LEN; i++)
	{
		cols[i] = my_rank + i * num_procs;
	}
	return 0;
}

int output_matrix()
{
	handle = netaware_fopen(fileName, MPI_MODE_RDWR);
	MPI_Offset offset;
	MPI_Aint type_size;
	MPI_Type_size(MPI_DOUBLE, &type_size);
	for (i = 0; i < MATRIX_LEN; i++)
	{
		offset = cols[i] * type_size;
		netaware_fwrite_at(handle, offset, cols[i], 1, MPI_DOUBLE);
	}
	netaware_fclose(handle);
	return 0;
}

int read_matrix()
{
	handle = netaware_fopen(fileName, MPI_MODE_RDWR);
	MPI_Offset offset;
	MPI_Aint type_size;
	MPI_Type_size(MPI_DOUBLE, &type_size);
	for (i = 0; i < MATRIX_LEN; i++)
	{
		offset = cols[i] * type_size;
		netaware_fread_at(handle, offset, cols[i], 1, MPI_DOUBLE);
	}
	netaware_fclose(handle);
	return 0;
}

int main(argc, argv)
int argc;
char * argv[];
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	init_cols();

	output_matrix();

	read_matrix();

	MPI_Finalize();
	exit(0);
}
