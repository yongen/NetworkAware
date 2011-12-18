#ifndef NETAWARE_ASYNC_SEGMENT_H
#define NETAWARE_ASYNC_SEGMENT_H

typedef struct segment_struct
{
	MPI_Offset offset;
	int length;
	char * data;
	struct segment_struct * next;
}* segment;

typedef struct segment_queue_struct
{
	struct segment_struct * head;
	struct segment_struct * tail;
}* segment_queue;

segment_queue init_segment_queue();
int insert_segment(segment_queue queue, segment item);
segment get_segment(segment_queue queue);

int netaware_async_segment_fwrite(netaware_async_fh handle);
int netaware_async_segment_fread(netaware_async_fh handle, char *buf,
				int count);

#endif
