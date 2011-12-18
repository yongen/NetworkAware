#ifndef NETAWARE_DELAY_H
#define NETAWARE_DELAY_H

typedef struct meta_struct
{
	MPI_Offset offset;
	int length;
	int file_id;
	MPI_Offset file_offset;

	struct meta_struct * next;
}* meta;

typedef struct meta_queue_struct
{
	struct meta_struct * head;
	struct meta_struct * tail;
	int size = 0;
}* meta_queue;

typedef struct meta_for_mpi_struct
{
	MPI_Offset offset;
	int length;
	int file_id;
	MPI_Offset file_offset;
}* meta_for_mpi;

typedef struct meta_for_merge_struct
{
	int count;
	struct meta_for_mpi_struct * array;
}* meta_for_merge;

meta_queue init_meta_queue();

int insert_meta(meta_queue queue, meta item);

meta_for_mpi get_meta_array(meta_queue queue);

meta_for_merge get_meta_for_merge_array(meta_queue queue);

MPI_Datatype get_meta_type();

MPI_Datatype get_meta_for_merge_type(meta_queue queue);

int meta_for_merge_function(meta_for_merge * in, meta_for_merge * inout, int * len,
		MPI_Datatype * type);

#endif
