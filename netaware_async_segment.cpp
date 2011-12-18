#include "netaware_async_segment.h"

segment_queue init_segment_queue()
{
	struct segment_queue_struct * queue = malloc(
			sizeof(struct segment_queue_struct));
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

int insert_segment(segment_queue queue, segment item)
{
	if (NULL == queue->head)
	{
		queue->head = item;
		queue->tail = item;
	}
	else
	{
		queue->tail->next = item;
		queue->tail = item;
	}
	return 0;
}
segment get_segment(segment_queue queue)
{
	segment tmp = queue->head;
	if (NULL != tmp)
	{
		queue->head = tmp->next;
	}
	return tmp;
}

int netaware_async_segment_fwrite(netaware_async_fh handle)
{
	segment item = malloc(sizeof(struct segment_struct));
	item->offset = handle->offset + handle->bfstart;
	item->length = handle->bfptr - handle->bfstart;
	item->data = malloc(sizeof(char) * item->length);
	memcpy(item->data, handle->data + handle->bfstart, item->length);
	item->next = NULL;
	insert_segment(handle->segment_queue, item);
	handle->bfstart = 0;
	return 0;
}

int netaware_async_segment_fread(netaware_async_fh handle, char *buf, int count)
{
	segment item = malloc(sizeof(struct segment_struct));
	item->offset = handle->offset + handle->bfptr;
	item->length = count;
	item->data = buf;
	insert_segment(handle->segment_queue, item);
	handle->bfptr += count;
	return 0;
}
