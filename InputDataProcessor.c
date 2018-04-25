/**
 * Name:
 *      InputDataProcessor.h
 *
 * Desc:
 *      Implementation of config and meta-data processor for the simulator
 *
 * Version:
 *      1.00 (17 Jan 2018)
 */

// Include guard
#ifndef INPUTDATAPROCESSOR_C
#define INPUTDATAPROCESSOR_C

// Include header
#include "InputDataProcessor.h"

// Constant for input buffer size
#define BUFFER_SIZE 255

// Constants for legal meta data tokens
const char META_LEGAL_COMPONENT_LETTERS[] = {'S', 'P', 'A', 'M', 'I', 'O'};
const char *META_LEGAL_OP_STRINGS[] = {"access", "allocate", "end",
                                       "hard drive", "keyboard", "printer",
                                       "monitor", "run", "start"};

// Begin implementation
CfgData *processCFG(char *fileName) {
    /**
     * Desc:
     *      Parse a CFG file into a data structure.
     *
     * Args:
     *      fileName (char*): file path of .cfg file
     *
     * Returns:
     *      If the config data file is successfully parsed, a CfgData struct
     *      is returned containing all relevant information
     *      (see InputDataProcessor.h for struct details).
     */
    CfgData *data = malloc( sizeof( CfgData ) ); // Allocate memory for the data
                                                 // structure

    FILE *config = fopen( fileName, "r" );       // Init file pointer

    char *buffer;                                // Init main buffer
    size_t len = 0;                              // Required for getline

    int dataRead[] = {0, 0, 0, 0, 0,            // Array of data we have hit
                       0, 0, 0, 0};              // in the parse loop

    int abort = 0;                               // Flag for aborting the parse
                                                 // if something breaks

    int hitStartFlag = 0;                     // Flag for when we hit
                                                 // "Start" flag in config file

    // Check to make sure config file exists
    if( config == NULL )
    {
        return NULL;
    }

    // Main parsing loop
    // Continually gets lines from the cfg, splits said string at the ':',
    // and parses data based on keywords
    while( 1 )
    {
        // Read line from file
        // If there is a read error abort
        if( getline( &buffer, &len, config ) == -1 )
        {
            return NULL;
        }

        // Remove the newline that getline() leaves in
        stripString( buffer );

        // Check if line read is the Start flag
        // If it is, parsing of data may begin
        if( ( compareString( buffer, "Start Simulator Configuration File" ) )
            == ( 1 ) )
        {
            hitStartFlag = 1;
            continue;
        }

        // If we haven't hit the start flag, keep reading lines without parsing
        if(hitStartFlag == 0)
        {
            continue;
        }

        // Check for the end flag for the cfg
        // If it is found, stop parsing
        if( ( compareString( buffer, "End Simulator Configuration File." ) )
            == ( 1 ) )
        {
            break;
        }

        // Split the line at the ':' with the left side being the data name
        // and the right side being the data value
        char configNameBuffer[BUFFER_SIZE];
        char configValueBuffer[BUFFER_SIZE];
        splitString( buffer, configNameBuffer, configValueBuffer, ':' );

        // Parse version number
        // Initially parse the expected double, then cast the value to an int
        // for storage
        if( ( compareString( configNameBuffer, "Version/Phase" ) ) == ( 1 ) )
        {
            data->version = (int) stringToDouble(configValueBuffer);

            // Make sure the data lies within spec bounds, otherwise abort
            if( ( data->version < 0 ) || ( data->version > 10 ) )
            {
                abort = 1;
            }

            dataRead[0] = 1;
        }

        // Parse MetaData file path
        if( ( compareString( configNameBuffer, "File Path" ) ) == ( 1 ) )
        {
            stripString( configValueBuffer );
            data->metaFilePath = copyString( configValueBuffer );
            dataRead[1] = 1;
        }

        // Parse CPU scheduling mode
        // Scheduling codes are defined via preprocessor directive in header
        if( ( compareString( configNameBuffer, "CPU Scheduling Code" ) )
            == ( 1 ) )
        {
            if( ( compareString( configValueBuffer, " NONE" ) ) == ( 1 ) )
            {
                data->cpuSchedulingCode = SCHED_NONE;
                dataRead[2] = 1;
            }
            else if( ( compareString( configValueBuffer, " FCFS-N" ) ) == ( 1 ) )
            {
                data->cpuSchedulingCode = SCHED_FCFS_N;
                dataRead[2] = 1;
            }
            else if( ( compareString( configValueBuffer, " SJF-N" ) ) == ( 1 ) )
            {
                data->cpuSchedulingCode = SCHED_SJF_N;
                dataRead[2] = 1;
            }
            else if( ( compareString( configValueBuffer, " SRTF-N" ) ) == ( 1 ) )
            {
                data->cpuSchedulingCode = SCHED_SRTF_P;
                dataRead[2] = 1;
            }
            else if( ( compareString( configValueBuffer, " FCFS-P" ) ) == ( 1 ) )
            {
                data->cpuSchedulingCode = SCHED_FCFS_P;
                dataRead[2] = 1;
            }
            else if( ( compareString( configValueBuffer, " RR-P" ) ) == ( 1 ) )
            {
                data->cpuSchedulingCode = SCHED_RR_P;
                dataRead[2] = 1;
            }
            else
            {
                abort = 1;
            }
        }

        // Parse quantum cycle time
        if( ( compareString( configNameBuffer, "Quantum Time (cycles)" ) )
            == ( 1 ) )
        {
            data->quantumTimeCycles = stringToInt( configValueBuffer );

            // Make sure the data is within the spec bounds
            if( ( data->quantumTimeCycles >= 100 ) ||
                ( data->quantumTimeCycles <= 0 ) )
            {
                abort = 1;
            }

            dataRead[3] = 1;
        }

        // Parse memory available
        if( ( compareString( configNameBuffer, "Memory Available (KB)" ) )
            == ( 1 ) )
        {
            data->memAvailable = stringToInt( configValueBuffer );

            // Make sure the data is within spec bounds, otherwise abort
            if( ( data->memAvailable < 1 ) || ( data->memAvailable > 1048576 ) )
            {
                abort = 1;
            }

            dataRead[4] = 1;
        }

        // Parse processor cycle time
        if( ( compareString( configNameBuffer, "Processor Cycle Time (msec)" ) )
            == ( 1 ) )
        {
            data->procCycleTime = stringToInt( configValueBuffer );

            // Make sure the data is within spec bounds, otherwise abort
            if( ( data->procCycleTime < 1 ) || ( data->procCycleTime > 10000 ) )
            {
                abort = 1;
            }

            dataRead[5] = 1;
        }

        // Parse I/O cycle time
        if( ( compareString( configNameBuffer, "I/O Cycle Time (msec)" ) )
            == ( 1 ) )
        {
            data->ioCycleTime = stringToInt( configValueBuffer );

            // Make sure the data is within spec bounds, otherwise abort
            if( ( data->ioCycleTime < 1 ) || ( data->ioCycleTime > 10000 ) )
            {
                abort = 1;
            }

            dataRead[6] = 1;
        }

        // Parse log mode
        // Log mode codes are defined via preprocessor directive in header
        if( ( compareString( configNameBuffer, "Log To" ) ) == ( 1 ) )
        {
            if( ( compareString( configValueBuffer, " Monitor" ) ) == ( 1 ) )
            {
                data->logTo = LOG_MONITOR;
                dataRead[7] = 1;
            } else if( ( compareString( configValueBuffer, " File" ) ) == ( 1 ) )
            {
                data->logTo = LOG_FILE;
                dataRead[7] = 1;
            } else if( ( compareString( configValueBuffer, " Both" ) ) == ( 1 ) )
            {
                data->logTo = LOG_BOTH;
                dataRead[7] = 1;
            } else
            {
                abort = 1;
            }
        }

        // Parse log file path
        if( ( compareString( configNameBuffer, "Log File Path" ) ) == ( 1 ) )
        {
            stripString( configValueBuffer );
            data->logPath = copyString(configValueBuffer);

            dataRead[8] = 1;
    	}

        if( ( abort ) == ( 1 ) )
        {
            if( ( data->logPath ) != ( NULL ) )
            {
                free( data->logPath );
            }

            if( ( data->metaFilePath ) != ( NULL ) )
            {
                free( data->metaFilePath );
            }

            free( buffer );
            free( data );
            fclose( config );
            return NULL;
        }
    }

    // If we haven't read in all the config data values needed, abort
    for( int readIndex = 0; ( readIndex ) < ( 9 ); readIndex++)
    {
        if( ( dataRead[readIndex] ) == ( 0 ) )
        {
            if( ( data->logPath ) != ( NULL ) )
            {
                free( data->logPath );
            }

            if( ( data->metaFilePath ) != ( NULL ) )
            {
                free( data->metaFilePath );
            }

            free( buffer );
            free( data );
            fclose( config );
            return NULL;
        }
    }

    // Clean up
    fclose( config );
    free( buffer );

    // Return struct after parsing is complete
    return data;
}

struct LinkedList *processMetaData(char *fileName) {
    /**
     * Desc:
     *      Parse a MetaData file into a data structure.
     *
     * Args:
     *      fileName (char*): Path of the .mdf file to parse
     *
     * Returns:
     *      If parsing is successful, the head of a linked list data structure
     *      is returned, which contains all relevant data
     */
    struct LinkedList *metaDataHead = linkedListCreate(NULL);
    if( ( fileName[0] ) == ( ' ' ) )
    {
        for( int index = 0; ( fileName[index] ) != ( '\0' ); index++ )
        {
            fileName[index] = fileName[index + 1];
        }
    }

    FILE *config = fopen(fileName, "r"); // Init file pointer
    char *buffer = (char *)malloc(BUFFER_SIZE);// Init main buffer
    size_t len = 0;                      // Required variables for
    int hitStartFlag = 0;                // Flag for when we hit "Start"
                                         // flag in metadata file

    // Make sure the file pointer isnt null
    if( ( config ) == ( NULL ) )
    {
        return NULL;
    }

    // Main parsing loop
    // Continually gets lines from the cfg, splits said string at the ':',
    // and parses data based on keywords
    while( 1 )
    {
        // Read line from file
        // If there is a read error abort
        if( ( getline( &buffer, &len, config ) ) == ( -1 ) )
        {
            break;
        }
        stripString( buffer );
        // Check if line read is the Start flag
        // If it is, parsing of data may begin
        if( ( compareString( buffer, "Start Program Meta-Data Code:" ) )
            == ( 1 ) )
        {
            hitStartFlag = 1;
            continue;
        }

        if( ( hitStartFlag ) == 0)
        {
            continue;
        }

        if( ( compareString( buffer, "End Program Meta-Data Code." ) ) == ( 1 ) )
        {
            break;
        }

        char left[BUFFER_SIZE];
        char right[BUFFER_SIZE];

        if( ( splitString( buffer, left, right, ';' ) ) != ( 1 ) )
        {
            break;
        }

        do
        {
            char *componentLetter = malloc( sizeof( char ) );
            char *opString = malloc( sizeof( char ) * BUFFER_SIZE );
            int  *cycleTime = malloc( sizeof( int ) );

            if( ( left[0] ) == ( ' ' ) )
            {
                for( int index = 0; ( left[index] ) != ( '\0' ); index++ )
                {
                    left[index] = left[index + 1];
                }
            }

            int elementsRead = sscanf(left, "%c(%[^)])%d", componentLetter,
                                       opString, cycleTime);

            int error_occured = 0;

            if( ( elementsRead ) != ( 3 ) )
            {
                error_occured = 1;
            }

            int found = 0;
            for( int index = 0; ( index ) < ( 6 ); index++ )
            {
                if( componentLetter != NULL &&
                    *componentLetter == META_LEGAL_COMPONENT_LETTERS[index] )
                {
                    found = 1;
                }
            }

            if( ( found ) != ( 1 ) )
            {
                error_occured = 1;
            }

            found = 0;
            for(int index = 0; ( index ) < ( 9 ); index++)
            {
                if( opString != NULL &&
                    compareString(opString, META_LEGAL_OP_STRINGS[index]) == 1 )
                {
                    found = 1;
                }
            }

            if(found != 1)
            {
                error_occured = 1;
            }

            // If there was a read error (eg unepected syntax) abort
            if( ( error_occured ) != ( 0 ) )
            {
                if( ( metaDataHead ) == ( NULL ) )
                {
                    return NULL;
                }
                else
                {
                    // Free up what was done in the linked list
                    while( ( metaDataHead ) != ( NULL ) )
                    {
                        if( ( metaDataHead->data ) != ( NULL ) )
                        {
                            free( metaDataHead->data[0] );
                            free( metaDataHead->data[1] );
                            free( metaDataHead->data[2] );
                            free( metaDataHead->data );
                        }

                        struct LinkedList *toDelete = metaDataHead;
                        metaDataHead = metaDataHead->next;
                        free( toDelete );
                    }

                    // Free up all malloc'd helper variables and close the file
                    // handle
                    free( componentLetter );
                    free( opString );
                    free( cycleTime );
                    free( buffer );
                    fclose( config );
                    return NULL;
                }
            }

            void **dataArray = malloc( sizeof( void* ) * 3 );
            dataArray[0] = componentLetter;
            dataArray[1] = opString;
            dataArray[2] = cycleTime;

            linkedListAppend( metaDataHead, dataArray );
        } while( ( splitString( right, left, right, ';' ) ) != ( 0 ) ||
                 ( splitString( right, left, right, '.' ) ) != ( 0 ) );
    }

    fclose( config );
    free( buffer );
    return metaDataHead;
}


// End Include guard
#endif              // INPUTDATAPROCESSOR_C
