#include <stdio.h>
#include "InputDataProcessor.h"
#include "LinkedList.h"
#include "ProcessPcb.h"
#include "simtimer.h"

// Constants to define schedule and log modes in the config
const char *CONFIG_SCHED_MODES[] = {"FCFS-N", "SJF-N", "SRTF-P",
                                    "FCFS-P", "RR-P"};
const char *CONFIG_LOG_MODES[] = {"MONITOR", "FILE", "BOTH"};

int main(int argc, char **argv) {
    if( ( argc ) < ( 2 ) )
    {
        fprintf( stderr, "Must specify config file path\n" );
        return -1;
    }

    CfgData *data =  processCFG( argv[1] );

    if( data == NULL )
    {
        fprintf( stderr, "There was an error parsing the config data file: %s\n",
                 argv[1] );
        return -1;
    }

    printf("___________BEGIN CFG DUMP___________\n");
    printf("Version             : %d\n", data->version);
    printf("Meta Data File Path : %s\n", data->metaFilePath);
    printf("CPU Scheduling Code : %s\n", CONFIG_SCHED_MODES[data->
                                             cpuSchedulingCode]);
    printf("Quantum Time Cycles : %d\n", data->quantumTimeCycles);
    printf("Memory Available    : %d\n", data->memAvailable);
    printf("Processor Cycle Time: %d\n", data->procCycleTime);
    printf("I/O Cycle Time      : %d\n", data->ioCycleTime);
    printf("Logging             : %s\n", CONFIG_LOG_MODES[data->logTo]);
    printf("Log File Path       : %s\n\n", data->logPath);

    struct LinkedList *meta = processMetaData( data->metaFilePath );

    int arraySize = getNumStartSymbols( meta->next );
    struct pcb *pcbArrayPointer = storePcb( meta, data );
    struct LogFile *logFile = processPcbArray(  pcbArrayPointer, data, arraySize );
    createLogFile( logFile, data->logPath );
    deallocatePcb( pcbArrayPointer );
    deallocateLogFile( logFile );

    if(meta == NULL)
    {
        fprintf(stderr, "There was an error parsing the meta data file: %s\n",
                data->metaFilePath);
        free(data->metaFilePath);
        free(data->logPath);
        free(data);
        return -1;
    }

    struct LinkedList *currentNode = meta->next;

    //printf("___________BEGIN META DUMP___________\n");
    while( currentNode != NULL )
    {
        /*printf( "The data item component letter is: %c\n",
                *(char*) currentNode->data[0] );
        printf( "The data item operation string is: %s\n",
                (char*) currentNode->data[1] );
        printf( "the data item cycle time is      : %d\n\n",
                *(int*) currentNode->data[2] );*/

        struct LinkedList *toDelete = currentNode;
        currentNode = currentNode->next;

        // Free the node now that we don't need it anymore
        free( toDelete->data[0] );
        free( toDelete->data[1] );
        free( toDelete->data[2] );
        free( toDelete->data );
        free( toDelete );
    }

    free( meta );                 // Free the linked list head
    free( data->metaFilePath );   // Free the cfg meta file
    free( data->logPath );        // Free the log file path
    free( data );                 // Free the rest of the cfgData struct
    return 0;
}
