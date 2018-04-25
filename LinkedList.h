/**
 * Name:
 *      LinkedList.h
 *
 * Desc:
 *      Struct and helper functions for a Generic, Singly-Linked list.
 *
 * Version:
 *      1.00 (27 Jan 2018)
 */

// Include Guard
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// Include header
#include <stdlib.h>
#include "StringUtilities.h"

// Linked list struct
struct LinkedList
{
    struct LinkedList *next;
    void **data;
};

struct LogFile
{
    struct LogFile *next;
    char *data;
};

struct Memory
{
    struct Memory *next;
    struct MMU *block;
};

// Function headers
struct LinkedList *linkedListCreate( void **data );
void linkedListAppend( struct LinkedList *head, void **data );
struct LogFile *logFileCreate();
void logFileAppend( struct LogFile *head, char *logLine );
struct Memory *memCreate();
void memAppend( struct Memory *head, struct MMU *block );


// End include guard
#endif              // LINKEDLIST_H
