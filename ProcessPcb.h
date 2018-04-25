/**
 * Name:
 *      Process.h
 *
 * Desc:
 *      Header file for Process.c
 *
 * Version:
 *      1.00 (27 Feb 2018)
 */

// Include Guard
#ifndef PROCESS_H
#define PROCESS_H
#define BUFFER_SIZE 255


// Include header
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "StringUtilities.h"
#include "LinkedList.h"
#include "InputDataProcessor.h"
#include "simtimer.h"


// Struct declaration
struct pcb
{
    struct LinkedList *metaDataPointer;
    int processCounter;
    int processCycle;
    char *processState;
};

struct MMU
{
    int processId;
    int segment;
    int base;
    int requested;
};

// Function headers

int getNumStartSymbols( struct LinkedList *head );
struct pcb *storePcb( struct LinkedList *metaData, CfgData *data );
struct pcb *reorderSJF( struct pcb *pcbArray, CfgData *data, int arraySize );
struct LogFile *processPcbArray( struct pcb *pcbArrayPointer, CfgData *data, int arraySize );
void startOperatingSystemOps(char *timeBuffer, struct LogFile *logFile, char *logLine, int logTo);
void setPcbArrayToReady( struct pcb *pcbArray, int arrayLength,
    struct LogFile *logFile, char *logLine, int logTo, char *timeBuffer );
void endOperatingSystemOps(char *timeBuffer, struct LogFile *logFile, char *logLine, int logTo);
void updatePointerAndCycle(struct pcb *pcbArrayPointer, int processNumber);
void addLineToLogFile(char *line, struct LogFile *listHead, int logTo);
void createLogFile( struct LogFile *listHead, char *logPath );
struct MMU *processMemoryRequest( struct pcb *memReq );
int allocateMem( struct MMU *mmu, struct Memory *memory, CfgData *data );
int accessMem( struct MMU *mmu, struct Memory *memory, CfgData *data );
int processPcbLine( CfgData *data, struct pcb *process, struct Memory *memory,
     char *stringTime, char *logLine, struct LogFile *logFile );
int estimatePcbTime(struct LinkedList *metaDataPointer, CfgData *data);
void *thread( void *ptr );
char *clearArray( char *arrayToClear );
void deallocateBuffers( char *timeBuffer, char *logLine );
void deallocatePcb( struct pcb *pcbArray );
void deallocateLogFile( struct LogFile *listHead );
void deallocateMemoryList( struct Memory *listHead );

// End include guard
#endif              // PROCESS_H
