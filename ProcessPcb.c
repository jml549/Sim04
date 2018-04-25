/**
 * Name:
 *      Process.c
 *
 * Desc:
 *      Parse the meta data file to create and process the Pcb's
 *
 * Version:
 *      1.00 (27 Feb 2018)
 */

// Include guard
#ifndef PROCESS_C
#define PROCESS_C

// Include header
#include "ProcessPcb.h"

const int BUFF_SIZE = 255;
const char *META_LEGAL_PROGRAM_STATES[] = {"New", "Waiting", "Running", "Exit"};
// Constants to define schedule and log modes in the config
const char *CONFIG_SCHED_MODES2[] = {"FCFS-N", "SJF-N", "SRTF-P",
                                    "FCFS-P", "RR-P"};

int getNumStartSymbols( struct LinkedList *head )
{
    struct LinkedList *currentNode = head->next;
    int count = 0;
    while( currentNode != NULL )
    {
        if( *(char*)currentNode->data[0] == 'A' &&
            compareString( currentNode->data[1], "start" ) == 1 )
        {
         count++;
        }
	      currentNode = currentNode->next;
    }
    return count;
}

struct pcb *storePcb( struct LinkedList *metaData, CfgData *data )
{
    int startSymbols = getNumStartSymbols(metaData->next);
    struct pcb *pcbArray = malloc(sizeof(struct pcb) * startSymbols);
    struct LinkedList *currentNode = metaData->next;
    int count = 0;

    while( currentNode != NULL && count < startSymbols)
    {
        if( *(char*)currentNode->data[0] == 'A' && compareString( (char*)currentNode->data[1], "start" ) )
        {
            struct pcb *newPcb = malloc(sizeof(struct pcb));
            newPcb->metaDataPointer = (struct LinkedList*)currentNode;
            newPcb->processCounter = count;
            newPcb->processCycle = *(int*)currentNode->data[2];
            newPcb->processState = "New";
            pcbArray[count] = *newPcb;
            currentNode = currentNode->next;
            count++;
            free(newPcb);
        }
	      currentNode = currentNode->next;
    }

    if( data->cpuSchedulingCode == 1 )
    {
        pcbArray = reorderSJF( pcbArray, data, count );
    }

    return pcbArray;
}

struct pcb *reorderSJF( struct pcb *pcbArray, CfgData *data, int arraySize )
{
    for(int outer = 0; outer < arraySize; outer++ )
    {
        for( int inner = 0; inner < arraySize; inner++ )
        {
            if( estimatePcbTime( pcbArray[outer].metaDataPointer, data ) <=
                estimatePcbTime( pcbArray[inner].metaDataPointer, data ))
            {
                struct pcb temp = pcbArray[outer];
                pcbArray[outer] = pcbArray[inner];
                pcbArray[inner] = temp;
            }
        }
    }
    return pcbArray;
}

struct LogFile *processPcbArray( struct pcb *pcbArrayPointer, CfgData *data, int arraySize )
{
    char *timeBuffer = (char *)malloc(BUFFER_SIZE);
    char *logLine = (char *)malloc(BUFFER_SIZE);
    struct LogFile *logFile = logFileCreate();

    startOperatingSystemOps(timeBuffer, logFile, logLine, data->logTo);
    setPcbArrayToReady(pcbArrayPointer, arraySize, logFile, logLine, data->logTo, timeBuffer);

    for(int processCount = 0; processCount < arraySize; processCount++)
    {
        struct Memory *memory = memCreate();
        sprintf(logLine, "Time:  %f, OS: %s Strategy selects Process %d with time: %d mSec\n",
            accessTimer(1, timeBuffer),
            CONFIG_SCHED_MODES2[data->cpuSchedulingCode],
            pcbArrayPointer[processCount].processCounter,
            estimatePcbTime( pcbArrayPointer[processCount].metaDataPointer, data ));
        addLineToLogFile(logLine, logFile, data->logTo);
        logLine = clearArray( logLine );

        //move pcb into running
        pcbArrayPointer[processCount].processState = "Running";
        sprintf(logLine, "Time:  %f, OS: Process %d set in %s state\n",
            accessTimer(1, timeBuffer),
            pcbArrayPointer[processCount].processCounter,
            pcbArrayPointer[processCount].processState);
        addLineToLogFile(logLine, logFile, data->logTo);
        logLine = clearArray( logLine );

        updatePointerAndCycle(pcbArrayPointer, processCount);

        //process all of the ops in a pcb
        while( *(char*)pcbArrayPointer[processCount].metaDataPointer->data[0] != 'A'
                && compareString( pcbArrayPointer[processCount].processState, "Running" )  )
        {
            //process one operation in the pcb
            if ( processPcbLine(data, &pcbArrayPointer[processCount], memory, timeBuffer, logLine, logFile) == 1 )
            {
                updatePointerAndCycle(pcbArrayPointer, processCount);
            }
            else
            {
                sprintf(logLine, "Time:  %f, OS: Process %d Segmentation Fault - Process ended\n",
                    accessTimer(1, timeBuffer),
                    pcbArrayPointer[processCount].processCounter);
                addLineToLogFile(logLine, logFile, data->logTo);
                break;
            }
        }

        //exit the process
        pcbArrayPointer[processCount].processState = "Exit";
        sprintf(logLine, "Time:  %f, OS: Process %d set in %s state\n",
            accessTimer(1, timeBuffer),
            pcbArrayPointer[processCount].processCounter,
            pcbArrayPointer[processCount].processState);
        addLineToLogFile(logLine, logFile, data->logTo);
        logLine = clearArray( logLine );
        deallocateMemoryList( memory );
    }

    endOperatingSystemOps( timeBuffer, logFile, logLine, data->logTo );
    deallocateBuffers( timeBuffer, logLine );

    return logFile;
}

void startOperatingSystemOps( char *timeBuffer, struct LogFile *logFile, char *logLine, int logTo )
{
    //handle operating system operations
    accessTimer( 0, timeBuffer );
    accessTimer( 1, timeBuffer );
    sprintf(logLine, "Time:  %f, OS: System Start\n", accessTimer( 1, timeBuffer ));
    addLineToLogFile( logLine, logFile, logTo );
    logLine = clearArray( logLine );
    sprintf( logLine, "Time:  %f, OS: Begin PCB Creation\n", accessTimer( 1, timeBuffer ) );
    addLineToLogFile( logLine, logFile, logTo );
    logLine = clearArray( logLine );
    sprintf( logLine, "Time:  %f, OS: All Processes initialized in New state\n",
        accessTimer( 1, timeBuffer ) );
    addLineToLogFile( logLine, logFile, logTo );
    logLine = clearArray( logLine );
}

void setPcbArrayToReady( struct pcb *pcbArray, int arrayLength,
    struct LogFile *logFile, char *logLine, int logTo, char *timeBuffer )
{
    for( int processCount = 0; processCount <= arrayLength; processCount++ )
    {
        pcbArray[processCount].processState = "Ready";
    }
    sprintf( logLine, "Time:  %f, OS: All Processes now set to Ready state\n",
        accessTimer( 1, timeBuffer ) );
    addLineToLogFile( logLine, logFile, logTo );
    logLine = clearArray( logLine );
}

void endOperatingSystemOps(char *timeBuffer, struct LogFile *logFile, char *logLine, int logTo)
{
    sprintf(logLine, "Time:  %f, OS: System stop\n", accessTimer( 1, timeBuffer ));
    addLineToLogFile( logLine, logFile, logTo );
    logLine = clearArray( logLine );
}

void updatePointerAndCycle( struct pcb *pcbArrayPointer, int processNumber )
{
    pcbArrayPointer[processNumber].metaDataPointer =
        pcbArrayPointer[processNumber].metaDataPointer->next;
    pcbArrayPointer[processNumber].processCycle =
        *(int*)pcbArrayPointer[processNumber].metaDataPointer->data[2];
}

void addLineToLogFile( char *line, struct LogFile *listHead, int logTo )
{
    switch ( logTo )
    {
        case 0: //monitor
            printf("%s", line);
            break;

        case 1: //file
            logFileAppend( listHead, line );
            break;

        case 2: //both
            logFileAppend( listHead, line );
            printf("%s", line);
            break;
    }
}

void createLogFile( struct LogFile *listHead, char *logPath )
{
    struct LogFile *currentNode = listHead->next;
    FILE *outFile = fopen( logPath, "w" );
    while( currentNode != NULL )
    {
        fprintf( outFile, "%s", (char*)currentNode->data );
        currentNode = currentNode->next;
    }
    fclose( outFile );
}

struct MMU *processMemoryRequest( struct pcb *memReq )
{
    int request = memReq->processCycle;
    struct MMU *newMmu = malloc( sizeof( struct MMU ) );

    newMmu->processId = memReq->processCounter;
    newMmu->segment = request/1000000;
    newMmu->base = (request/1000)%1000;
    newMmu->requested = request%1000;

    return newMmu;
}

int allocateMem( struct MMU *mmu, struct Memory *memory, CfgData *data )
{
    struct Memory *currentBlock = memory->next;

    if( currentBlock == NULL && mmu->base + mmu->requested <= data->memAvailable )
    {
        memAppend( memory, mmu );
        return 1;
    }

    while( currentBlock != NULL )
    {
        if( ( mmu->base < currentBlock->block->base
            || mmu->base > currentBlock->block->base )
            && ( mmu->base + mmu->requested <= currentBlock->block->base
            || mmu->base + mmu->requested >= currentBlock->block->base + currentBlock->block->requested )
            && currentBlock->block->base + mmu->requested <= data->memAvailable )
            {
                continue;
            }
            else
            {
                return 0;
            }
        currentBlock = currentBlock->next;
    }
    return 1;
}

int accessMem( struct MMU *mmu, struct Memory *memory, CfgData *data )
{
    struct Memory *currentBlock = memory->next;

    if( currentBlock == NULL )
    {
        return 0;
    }

    while( currentBlock != NULL )
    {
        if( ( mmu->base >= currentBlock->block->base && mmu->base
            < currentBlock->block->base + currentBlock->block->requested )
            && ( mmu->base + mmu->requested >= currentBlock->block->base
            && mmu->base + mmu->requested < currentBlock->block->base + currentBlock->block->requested )
            && mmu->base + mmu->requested <= data->memAvailable )
            {
                continue;
            }
            else
            {
                return 0;
            }
        currentBlock = currentBlock->next;
    }
    return 1;
}



int processPcbLine( CfgData *data, struct pcb *process, struct Memory *memory,
     char *stringTime, char *logLine, struct LogFile *logFile )
{
    char opCode = *(char*)process->metaDataPointer->data[0];
    char *opString = (char*)process->metaDataPointer->data[1];
    int logTo = data->logTo;
    int ioWait = data->ioCycleTime * process->processCycle;
    int procWait = data->procCycleTime * process->processCycle;


    pthread_t thread1;

    switch( opCode )
    {
        case 'M':;
            struct MMU *mmu = processMemoryRequest( process );
            logLine = clearArray( logLine );
            if( compareString( opString, "allocate" ) == 1 )
            {
                sprintf( logLine, "Time:  %f, Process %d, MMU Allocation: %d/%d/%d start \n",
                    accessTimer( 1, stringTime ),
                    process->processCounter,
                    mmu->segment,
                    mmu->base,
                    mmu->requested );
                addLineToLogFile( logLine, logFile, logTo );
                logLine = clearArray( logLine );

                if( allocateMem( mmu, memory, data ) == 1 )
                {
                    sprintf( logLine, "Time:  %f, Process %d, MMU Allocation: Successful\n",
                        accessTimer( 1, stringTime ),
                        process->processCounter );
                    addLineToLogFile( logLine, logFile, logTo );
                    logLine = clearArray( logLine );
                    return 1;
                }
                else
                {
                    sprintf( logLine, "Time:  %f, Process %d, MMU Allocation: Failed\n",
                        accessTimer( 1, stringTime ),
                        process->processCounter );
                    addLineToLogFile( logLine, logFile, logTo );
                    logLine = clearArray( logLine );
                    return 0;
                }
            }
            else if( compareString( opString, "access" ) == 1 )
            {
                sprintf( logLine, "Time:  %f, Process %d, MMU Access: %d/%d/%d start \n",
                    accessTimer( 1, stringTime ),
                    process->processCounter,
                    mmu->segment,
                    mmu->base,
                    mmu->requested );
                addLineToLogFile( logLine, logFile, logTo );
                logLine = clearArray( logLine );
                if( accessMem( mmu, memory, data ) == 1 )
                {
                    sprintf( logLine, "Time:  %f, Process %d, MMU Access: Successful\n",
                        accessTimer( 1, stringTime ),
                        process->processCounter );
                    addLineToLogFile( logLine, logFile, logTo );
                    logLine = clearArray( logLine );
                    free(mmu);
                    return 1;
                }
                else
                {
                    sprintf( logLine, "Time:  %f, Process %d, MMU Access: Failed\n",
                        accessTimer( 1, stringTime ),
                        process->processCounter );
                    addLineToLogFile( logLine, logFile, logTo );
                    logLine = clearArray( logLine );
                    free( mmu );
                    return 0;
                }
            }

        case 'I':
            logLine = clearArray( logLine );
            sprintf( logLine, "Time:  %f, Process %d, %s input start \n",
                accessTimer( 1, stringTime ),
                process->processCounter,
                opString );
            addLineToLogFile( logLine, logFile, logTo );
            logLine = clearArray( logLine );
            pthread_create( &thread1, NULL, *thread, &ioWait );
            pthread_join( thread1, NULL );
            sprintf( logLine, "Time:  %f, Process %d, %s input end \n",
                accessTimer( 1, stringTime ),
                process->processCounter,
                opString );
            addLineToLogFile( logLine, logFile, logTo );
            logLine = clearArray( logLine );
            return 1;

        case 'O':
            logLine = clearArray( logLine );
            sprintf( (char*)logLine, "Time:  %f, Process %d, %s output start \n",
                accessTimer( 1, stringTime ),
                process->processCounter,
                opString );
            addLineToLogFile( logLine, logFile, logTo );
            logLine = clearArray( logLine );
            pthread_create( &thread1, NULL, *thread, &ioWait );
            pthread_join( thread1, NULL );
            sprintf( (char*)logLine, "Time:  %f, Process %d, %s output end \n",
                accessTimer( 1, stringTime ),
                process->processCounter,
                opString );
            addLineToLogFile( logLine, logFile, logTo );
            logLine = clearArray( logLine );
            return 1;

        case 'P':
            logLine = clearArray( logLine );
            sprintf( logLine, "Time:  %f, Process %d, %s operation start \n",
                accessTimer( 1, stringTime ),
                process->processCounter,
                opString );
            addLineToLogFile(logLine, logFile, logTo);
            logLine = clearArray( logLine );
            runTimer( procWait );
            sprintf( logLine, "Time:  %f, Process %d, %s operation end \n",
                accessTimer(1, stringTime),
                process->processCounter,
                opString );
            addLineToLogFile(logLine, logFile, logTo);
            logLine = clearArray( logLine );
            return 1;

        default:
            return 0;
    }
}

int estimatePcbTime( struct LinkedList *metaDataPointer, CfgData *data )
{
    while( *(char*)metaDataPointer->data[0] == 'A' )
    {
        metaDataPointer = metaDataPointer->next;
    }

    int timeEstimate = 0;
    while( metaDataPointer != NULL && *(char*)metaDataPointer->data[0] != 'A' )
    {
        if( *(char*)metaDataPointer->data[0] == 'I' || *(char*)metaDataPointer->data[0] == 'O' )
        {
            timeEstimate += ( *(int*)metaDataPointer->data[2]*data->ioCycleTime );
        }
        else if( *(char*)metaDataPointer->data[0] == 'P' )
        {
            timeEstimate += ( *(int*)metaDataPointer->data[2]*data->procCycleTime );
        }

        metaDataPointer = metaDataPointer->next;
    }

    return timeEstimate;
}

void *thread( void *ptr )
{
    runTimer( *(int*)ptr );
    return ptr;
}

char *clearArray( char *arrayToClear )
{
    for( int index = 0; index < BUFF_SIZE; index++ )
    {
        arrayToClear[index] = '\0';
    }
    return (char*)arrayToClear;
}

void deallocateBuffers( char *timeBuffer, char *logLine )
{
    free(timeBuffer);
    free(logLine);
}

void deallocatePcb( struct pcb *pcbArray )
{
    free( pcbArray );
}

void deallocateLogFile( struct LogFile *listHead )
{
    struct LogFile *currentNode = listHead->next;
    while( currentNode != NULL )
    {
        struct LogFile *toDelete = currentNode;
        currentNode = currentNode->next;
        free( toDelete->data );
        free( toDelete );
    }

    free( listHead );
}

void deallocateMemoryList( struct Memory *listHead )
{
    struct Memory *currentNode = listHead->next;
    while( currentNode != NULL )
    {
        struct Memory *toDelete = currentNode;
        currentNode = currentNode->next;
        free( toDelete->block );
        free( toDelete );
    }

    free( listHead );
}



// End Include guard
#endif              // PROCESS_C
