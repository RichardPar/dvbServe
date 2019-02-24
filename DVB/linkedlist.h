#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

#include <pthread.h>

typedef struct linked_node
{
	void *next;
	void *prev;
	void *data;
}
linked_node;


typedef struct linked_list
{
	int count;
	int ( *cmp ) ( void *, void* );
	pthread_mutex_t Lock;
	void      *headerPtr;
	void      *footerPtr;
	linked_node *item;
}
linked_list;

typedef struct linked_iterator
{
	struct linked_list *original;
	void *next;
	void *prev;
	void *current;
}
linked_iterator;

// Internal functions to Wrap the exported ones

void *linkedList_create_iterator ( struct linked_list *list,struct linked_iterator *itr );
void *linkedList_iterate ( struct linked_list *list,struct linked_iterator *iterator );
void *linkedList_iterator_remove_internal ( struct linked_list *list, struct linked_iterator *node );
void *linkedList_find_internal ( struct linked_list *list,void *inData );
linked_list *linkedList_create ( void *compareFunc );
void *linkedList_iterator_remove ( struct linked_list *list, struct linked_iterator *node );
void *linkedList_remove_internal ( struct linked_list *list,void *inData );
void linkedList_release_iterator ( struct linked_list *list,struct linked_iterator *itr );
void *linkedList_create_iterator_internal ( struct linked_list *list,struct linked_iterator *itr );
void linkedList_lock ( struct linked_list *list );
void linkedList_unlock ( struct linked_list *list );
int linkedList_append_internal ( struct linked_list *list, void *item );
void *linkedList_iterator_remove_internal ( struct linked_list *list, struct linked_iterator *node );
int linkedList_scan_internal ( struct linked_list *list,void *inData );
int linkedList_item_count ( struct linked_list *list );
int linkedList_append ( struct linked_list *list, void *item );
void *linkedList_create_iterator_internal ( struct linked_list *list,struct linked_iterator *itr );
int linkedList_insert ( struct linked_list *list, void *item );
int linkedList_insert_internal ( struct linked_list *list, void *item );
void linkedList_destroy ( struct linked_list *list );

#endif

