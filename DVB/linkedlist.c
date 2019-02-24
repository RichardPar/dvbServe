
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "linkedlist.h"


linked_list *linkedList_create ( void *compareFunc )
{
	linked_list *outList;
	outList=malloc ( sizeof ( struct linked_list ) );
	if (outList == NULL)
      	{
	printf("Out of memory to create linked list\r\n");
        return NULL;
	}
	outList->cmp=compareFunc;
	outList->headerPtr=NULL;
	outList->footerPtr=NULL;
	outList->item=NULL;
	outList->count=0;
	pthread_mutex_init ( &outList->Lock,NULL );
	return outList;
}

void linkedList_destroy ( struct linked_list *list )
{
	linked_list *outList = list;


	if ( list->count != 0 )
	{
		printf ( "Linked list is not empty!!! please clear %i rentries before delete - MEMORY WILL Leak\r\n",list->count );
		return;

	}
	outList->headerPtr=NULL;
	outList->footerPtr=NULL;
	outList->item=NULL;
	outList->count=0;
	pthread_mutex_destroy ( &outList->Lock );
	free ( outList );
}

int linkedList_append ( struct linked_list *list, void *item )
{
	pthread_mutex_lock ( &list->Lock );
	int rc = linkedList_append_internal ( list,item );
	pthread_mutex_unlock ( &list->Lock );
	return rc;
}

int linkedList_insert ( struct linked_list *list, void *item )
{
	pthread_mutex_lock ( &list->Lock );
	int rc = linkedList_insert_internal ( list,item );
	pthread_mutex_unlock ( &list->Lock );
	return rc;
}

int linkedList_remove ( struct linked_list *list, void *item )
{
	pthread_mutex_lock ( &list->Lock );
	void *rc = linkedList_remove_internal ( list,item );
	pthread_mutex_unlock ( &list->Lock );
        if (rc == NULL)
           return 0;
	return 1;
}

void *linkedList_remove_internal ( struct linked_list *list,void *inData )
{
	int cmpRet;
	if ( list->cmp == NULL )
		return NULL;

	struct linked_iterator iterator;

	char *retData = linkedList_create_iterator_internal ( list,&iterator );
	while ( retData != NULL )
	{
		cmpRet =  list->cmp ( retData,inData );
		if ( cmpRet == 0 )
		{
			retData = linkedList_iterator_remove_internal ( list,&iterator );
		}
		else
			retData = linkedList_iterate ( list,&iterator );
	}

	return NULL;
}


void *linkedList_find_internal ( struct linked_list *list,void *inData )
{
	int cmpRet=-1;
	if ( list->cmp == NULL )
		return NULL;

	struct linked_iterator iterator;

	char *retData = linkedList_create_iterator_internal ( list,&iterator );
	while ( retData != NULL )
	{
		cmpRet =  list->cmp ( retData,inData );
		if ( cmpRet == 0 )
			break;
		retData = linkedList_iterate ( list,&iterator );
	}
	if ( cmpRet == 0 )
		return retData;

	return NULL;
}


void *linkedList_find ( struct linked_list *list, void *item )
{
	void *retPtr;
	retPtr = linkedList_find_internal ( list,item );
	return retPtr;
}

int linkedList_append_internal ( struct linked_list *list, void *item )
{
	struct linked_node *newItem;


	if ( ( list->headerPtr != NULL ) && ( list->footerPtr != NULL ) )
	{
		int rc = linkedList_scan_internal ( list,item );
		if ( rc==0 )
		{
			printf ( "Duplicate Entry\r\n" );
			return -1;
		}
	}

	newItem = malloc ( sizeof ( struct linked_node ) );
	if (newItem == NULL)
      	{
	printf("Out of memory to create new item\r\n");
        return -1;
	}
	memset ( newItem,0,sizeof ( struct linked_node ) );
	newItem->data = item;
	newItem->next=NULL;
	newItem->prev=list->footerPtr;
	if ( list->headerPtr==NULL )
	{
		list->item=newItem;
		list->headerPtr=newItem;
		list->footerPtr = newItem;
	}
	else
	{
		struct linked_node *endItem;
		endItem = list->footerPtr;
		endItem->next=newItem;
		list->footerPtr = newItem;
	}
	list->count++;
	return 1;
}

int linkedList_insert_internal ( struct linked_list *list, void *item )
{
	struct linked_node *newItem;


	if ( ( list->headerPtr != NULL ) && ( list->footerPtr != NULL ) )
	{
		int rc = linkedList_scan_internal ( list,item );
		if ( rc==0 )
		{
			printf ( "Duplicate Entry\r\n" );
			return -1;
		}
	}

	newItem = malloc ( sizeof ( struct linked_node ) );
	if (newItem == NULL)
      	{
	printf("Out of memory to create new item\r\n");
        return -1;
	}
	memset ( newItem,0,sizeof ( struct linked_node ) );
	newItem->data = item;

        newItem->prev=NULL;
	newItem->next=list->headerPtr;

	if ( list->headerPtr==NULL )
	{
		list->item=newItem;
		list->headerPtr=newItem;
		list->footerPtr=newItem;
	}
	else
	{
		struct linked_node *firstItem;
		firstItem = list->headerPtr;
		firstItem->prev=newItem;
		list->headerPtr = newItem;
	}
	list->count++;
	return 1;
}



int linkedList_scan_internal ( struct linked_list *list,void *inData )
{
	int cmpRet=-1;
	if ( list->cmp == NULL )
		return -1;

	struct linked_iterator iterator;

	char *retData = linkedList_create_iterator_internal ( list,&iterator );
	while ( retData != NULL )
	{
		cmpRet =  list->cmp ( retData,inData );
		if ( cmpRet == 0 )
			break;
		retData = linkedList_iterate ( list,&iterator );
	}
	return cmpRet;
}

void linkedList_release_iterator ( struct linked_list *list,struct linked_iterator *itr )
{
	memset ( itr,0,sizeof ( struct linked_iterator ) );
	linkedList_unlock ( list );
	return;
}

void *linkedList_create_iterator ( struct linked_list *list,struct linked_iterator *itr )
{
	linkedList_lock ( list );
	return linkedList_create_iterator_internal ( list,itr );
}

void *linkedList_create_iterator_internal ( struct linked_list *list,struct linked_iterator *itr )
{
	struct linked_node *item;

	memset ( itr,0,sizeof ( struct linked_iterator ) );
	item=list->headerPtr;
	if ( item != NULL )
	{
		itr->next = item->next;
	}
	else
		itr->next=NULL;

	itr->prev=NULL;
	itr->current=list->headerPtr;
	itr->original=list;
        //printf("Iterator Created [C=%p][N=%p][P=%p]\r\n",itr->current,itr->next,itr->prev);
	if ( item != NULL )
		return item->data;

	return NULL;
}

void *linkedList_iterate ( struct linked_list *list,struct linked_iterator *iterator )
{


	struct linked_node *next;
	struct linked_iterator *itr=iterator;

	if (itr->next==NULL)
                  return NULL;
        
	//current = itr->current;
	//prev=itr->prev;
	next=itr->next;
	itr->current=next; 
	itr->prev=next->prev;
    itr->next=next->next;

	return next->data;
}


void linkedList_lock ( struct linked_list *list )
{
	pthread_mutex_lock ( &list->Lock );
	return;
}

void linkedList_unlock ( struct linked_list *list )
{
	pthread_mutex_unlock ( &list->Lock );
	return;
}

void *linkedList_iterator_remove ( struct linked_list *list, struct linked_iterator *node )
{
	pthread_mutex_lock ( &list->Lock );
	void *outList = linkedList_iterator_remove_internal ( list,node );
	pthread_mutex_unlock ( &list->Lock );
	return outList;
}

int linkedList_item_count ( struct linked_list *list )
{
	int rc;
	pthread_mutex_lock ( &list->Lock );
	rc = list->count;
	pthread_mutex_unlock ( &list->Lock );
	return rc;
}

void *linkedList_iterator_remove_internal ( struct linked_list *list, struct linked_iterator *node )
{
	struct linked_node *current,*prev,*next;
	struct linked_iterator *itr=node;

	if ( node->current==NULL )
		return NULL;

	current = itr->current;
	prev=itr->prev;
	next=itr->next;

	if ( next != NULL )
	{
		itr->next=next->next;
	}
	else
		itr->next=NULL;

	itr->current=current->next;

	if ( prev != NULL )
	{
		itr->prev=prev;
	}
	else
		itr->prev=NULL;


	if ( current==list->headerPtr )
	{
		list->headerPtr=current->next;
		if ( current->next != NULL )
		{
			next->prev=NULL;
		}
		itr->prev = NULL;
	}

	if ( current==list->footerPtr )
	{
		list->footerPtr=current->prev;
		if ( current->prev != NULL )
		{
			prev->next = NULL;
		}
		itr->next=NULL;
	}

	free ( current );
	list->count--;

	if ( ( list->headerPtr == NULL ) && ( list->footerPtr == NULL ) )
		return NULL;

	if ( ( next != NULL ) && ( prev != NULL ) )
	{
		next->prev=prev;
		prev->next=next;
	}
	if ( itr->current == NULL )
		itr->prev=NULL;

	if ( next != NULL )
		return next->data;

	return NULL;
}


