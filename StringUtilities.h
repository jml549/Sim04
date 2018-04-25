/**
 * Name:
 *      StringUtilities.h
 *
 * Desc:
 *      Small library of string utilities.
 *
 * Version:
 *      1.00 (18 Jan 2018)
 */

// Include guard
#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H

// Needed libraries
#include <stdlib.h>
#include <math.h>

// Function headers
int compareString( const char *str1, const char *str2 );
int splitString( char *str, char *left, char *right, char delimiter );
char *copyString( const char *str );
int stringToInt( const char *str );
double stringToDouble( const char *str );
void stripString( char *str );

// End include guard
#endif              // STRINGUTILITIES_H
