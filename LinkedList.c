/**
 * Name:
 *      LinkedList.c
 *
 * Desc:
 *      Implementation of LinkedList.h
 *
 * Version:
 *      1.00 (18 Jan 2018)
 */

// Include Guard
#ifndef LINKEDLIST_C
#define LINKEDLIST_C

// Include header file
#include "LinkedList.h"

// Begin implementation
struct LinkedList *linkedListCreate( void **data )
{
    /**
     * Desc:
     *      Setup the head of a linked list.
     *
     * Args:
     *      data (void**): array of void pointers.
     *
     * Returns:
     *      Pointer to a struct linked_list with data inserted.
     */
    struct LinkedList *list = malloc( sizeof( struct LinkedList ) );
    list->data = data;
    list->next = NULL;
    return list;
}

void linkedListAppend( struct LinkedList *head, void **data )
{
    /**
     * Desc:
     *      Create a new node and append it to a linked list.
     *
     * Args:
     *      head (struct linked_list*): Pointer to the head of the linked list.
     *      data (void**): Void pointer array of data to add.
     */
    if( ( head->next ) == ( NULL ) )
    {
        head->next = (struct LinkedList*) malloc( sizeof( struct LinkedList));
        head->next->data = data;
        head->next->next = NULL;
        return;
    }
    else
    {
        linkedListAppend( head->next, data );
        return;
    }
}

struct LogFile *logFileCreate()
{
    struct LogFile *list = malloc( sizeof( struct LogFile ) );
    list->next = NULL;
    return list;
}

void logFileAppend( struct LogFile *head, char *logLine )
{
    if( ( head->next ) == ( NULL ) )
    {
        head->next = (struct LogFile*) malloc( sizeof( struct LogFile));
        head->next->data = copyString( logLine );
        head->next->next = NULL;
        return;
    }
    else
    {
        logFileAppend( head->next, logLine );
        return;
    }
}

struct Memory *memCreate()
{
    struct Memory *memory = malloc( sizeof( struct Memory ) );
    memory->next = NULL;
    return memory;
}

void memAppend( struct Memory *head, struct MMU *block )
{
    if( ( head->next ) == ( NULL ) )
    {
        head->next = (struct Memory*) malloc( sizeof( struct Memory ) );
        head->next->block = block;
        head->next->next = NULL;
        return;
    }
    else
    {
        memAppend( head->next, block );
        return;
    }
}


// End include guard
#endif              // LINKEDLIST_C
