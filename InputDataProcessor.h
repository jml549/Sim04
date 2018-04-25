/**
 * Name:
 *      InputDataProcessor.h
 *
 * Desc:
 *      Header for Config and meta-data processor for the simulator
 *
 * Version:
 *      1.00 (17 Jan 2018)
 */

// Include guard
#ifndef INPUTDATAPROCESSOR_H
#define INPUTDATAPROCESSOR_H

// Preprocessor directives for CPU scheduling
#define SCHED_NONE   0;
#define SCHED_FCFS_N 0;
#define SCHED_SJF_N  1;
#define SCHED_SRTF_P 2;
#define SCHED_FCFS_P 3;
#define SCHED_RR_P   4;

// Preprocessor directives for logging
#define LOG_MONITOR 0;
#define LOG_FILE    1;
#define LOG_BOTH    2;

// Imported files
#include <stdio.h>
#include <inttypes.h>
#include "StringUtilities.h"
#include "LinkedList.h"

// CFG file data struct
typedef struct {
    int version;
    char *metaFilePath;
    int cpuSchedulingCode;
    int quantumTimeCycles;
    int memAvailable;
    int procCycleTime;
    int ioCycleTime;
    int logTo;
    char *logPath;
} CfgData;

// Function headers
CfgData *processCFG(char *fileName);
struct LinkedList *processMetaData(char *fileName);

// End include guard
#endif              // INPUTDATAPROCESSOR_C
