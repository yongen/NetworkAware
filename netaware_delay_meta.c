#include "netaware_delay_meta.h"

meta_queue init_meta_queue()
{
	meta_queue queue = malloc(sizeof(struct meta_queue_struct));
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

int insert_meta(meta_queue queue, meta item)
{
	if (NULL == queue->head)
	{
		queue->head = item;
		queue->tail = item;
	}
	else if (queue->tail->offset < item->offset)
	{
		queue->tail->next = item;
		queue->tail = item;
	}
	else if (queue->head->offset > item->offset)
	{
		item->next = queue->head;
		queue->head = item;
	}
	else
	{
		meta pre;
		meta tmp = queue->head;
		while (NULL != tmp && tmp->offset < item->offset)
		{
			pre = tmp;
			tmp = tmp->next;
		}
		pre->next = item;
		item->next = tmp;
	}
	queue->size++;
	return 0;
}

meta_for_mpi get_meta_array(meta_queue queue)
{
	int i = 0;
	meta_for_mpi array = malloc(sizeof(struct meta_for_mpi_struct)
			* queue->size);
	meta tmp;
	meta item = queue->head;
	while (NULL != item)
	{
		array[i].offset = item->offset;
		array[i].length = item->length;
		array[i].file_id = item->file_id;
		array[i].file_offset = item->file_offset;
		tmp = item;
		item = item->next;
		i++;
		free(tmp);
	}
	queue->head = NULL;
	queue->tail = NULL;
	return array;
}

MPI_Datatype get_meta_type()
{
	MPI_Datatype meta_type;
	struct meta_for_mpi_struct meta;
	int lens[] =
	{ 1, 1, 1, 1 };
	MPI_Datatype types[] =
	{ MPI_Offset, MPI_INT, MPI_INT, MPI_Offset };

	MPI_Aint disp[4];
	disp[0] = &meta.offset - &meta;
	disp[1] = &meta.length - &meta;
	disp[2] = &meta.file_id - &meta;
	disp[3] = &meta.file_offset - &meta;
	MPI_Type_create_struct(4, lens, disp, types, &meta_type);
	MPI_Type_commit(&meta_type);
	return meta_type;
}

meta_for_merge get_meta_for_merge_array(meta_queue queue)
{
	meta_for_merge result= malloc(sizeof(struct meta_for_merge_struct));
	result->count = queue->size;
	result->array = get_meta_array(queue);
	return result;
}

MPI_Datatype get_meta_for_merge_type(meta_queue queue)
{
	MPI_Datatype merge_type;
	int lens[2];
	lens[1] = 1;
	lens[2] = queue->size;

	MPI_Datatype types[2];
	types[0] = MPI_INT;
	types[1] = get_meta_type();
	struct meta_for_merge_struct meta_for_merge;

	MPI_Aint disp[2];
	disp[0] = &meta_for_merge.count-&meta_for_merge;
	disp[1] = &meta_for_merge.array-&meta_for_merge;
	MPI_Type_create_struct(2, lens, disp, types, &merge_type);
	MPI_Type_commit(&merge_type);
	return merge_type;
}

void meta_for_merge_function(meta_for_merge * in, meta_for_merge * inout, int * len,
		MPI_Datatype * type)
{
	int in_count = (*in)->count;
	int inout_count = (*inout)->count;
	int size = sizeof(struct meta_for_mpi_struct) * (in_count+inout_count);
	meta_for_mpi result = malloc(size);

	int i = 0;
	int j = 0;
	int k = 0;
	while (i < in_count && j < inout_count)
	{
		if ((*in)[i].offset < (*inout)[j].offset)
		{
			result[k++] = (*in)[i++];
		}
		else
		{
			result[k++] = (*inout)[j++];
		}
	}
	while (i < in_count)
	{
		result[k++] = (*in)[i++];
	}
	while (j < inout_count)
	{
		result[k++] = (*inout)[j++];
	}
	free(*inout);
	meta_for_merge re = malloc(sizeof(meta_for_merge_struct));
	re->count = in_count+inout_count;
	re->array = result;
	inout = &re;
}
