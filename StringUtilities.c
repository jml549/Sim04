/**
 * Name:
 *      StringUtilities.c
 *
 * Desc:
 *      Implementation of string utlity functions
 *
 * Version:
 *      1.00 (18 Jan 2018)
 *
 * Notes:
 *      These utilty functions were made to assist with OS Simulator ops. They
 *      probably have a lot of undefined behaviour and belong on the computer
 *      version of a shortbus, so be gentle.
 */

// Include guard
#ifndef STRINGUTILITIES_C
#define STRINGUTILITIES_C

// Includes
#include "StringUtilities.h"

// Begin Implementation
int compareString( const char *str1, const char *str2 )
{
    /**
     * Desc:
     *      Compare two null-terminated strings.
     *
     * Args:
     *      str1 (char*): String 1
     *      str2 (char*): String 2
     *
     * Returns:
     *      If both strings have the same chars UP TO THE NULL TERMINATOR, 0.
     *      Otherwise, -1.
     */
    int strCmpInd = 0;
    while( 1 ) 
    {
        if( ( str1[strCmpInd] ) == ( str2[strCmpInd] ) ) 
        {
            if( ( str1[strCmpInd] ) == ( '\0' ) && 
                ( str2[strCmpInd] ) == ( '\0' ) ) 
            {
                return 1;
            } 
            else 
            {
                strCmpInd++;
            }
        } 
        else 
        {
            return 0;
        }
    }
}

int splitString( char *buffer, char *left, char *right, char delimiter )
{
    /**
     * Desc:
     *      Takes a NULL TERMINATED string, and splits it in half,
     *      using delimiter as a pivot. The char that was pivoted on
     *      is NOT STORED.
     *
     * Args:
     *      buffer (char*): original buffer to read chars out of
     *      left (char*): chars in string that are left of the pivot
     *      right (char*): chars in string that are right of the pivot
     *      delimiter (const char*): string to be used as a delimiter
     *
     * Returns:
     *      If the delimiter pivot is found in buffer, 0 is returned.
     *      If a pivot is not found, -1.
     */
    int bufferIndex = 0;
    int splitIndex = 0;

    // Loop for left
    while( 1 )
    {
        if( ( buffer[bufferIndex] ) == ( delimiter ) ) 
        {
            left[bufferIndex] = '\0';
            bufferIndex++;
            splitIndex = bufferIndex;
            break;
        } 
        
        // If we've reached the end of the buffer string, return -1
        if( ( buffer[bufferIndex] ) == ( '\0' ) )
        {
            left[bufferIndex] = '\0';
            return 0;
        }

        // copy current char to left
        left[bufferIndex] = buffer[bufferIndex];
        bufferIndex++;
    }

    // Loop for right
    while( 1 )
    {
        // If we've reached the end of the string, return 0
        if( ( buffer[bufferIndex] ) == ( '\0' ) )
        {
            right[bufferIndex - splitIndex] = '\0';
            return 1;
        }

        right[bufferIndex - splitIndex] = buffer[bufferIndex];
        bufferIndex++;
    }
}

char *copyString( const char *str )
{
    /**
     * Desc:
     *      Creates a copy of a null terminated string on the heap.
     *
     * Args:
     *      str (const char*): String to copy
     *
     * Returns:
     *      If str is not a null pointer, a pointer to a NULL TERMINATED
     *      string is returned. Otherwise, a null pointer is returned.
     */
    // If str is a null pointer, return null pointer
    if( !str )
    {
        return (char*) str;
    }

    // Get the length of the string up to the null terminator
    int stringLength = 0;
    while( 1 )
    {
        if( ( str[stringLength++] ) != ( '\0' ) ) 
        {
            continue;
        }
        else 
        {
            break;
        }
    }

    // Allocate heap space for copied string
    char *copy = malloc( sizeof( char ) * stringLength);

    for( int index = 0; ( index )  < ( stringLength ); index++ )
    {
        copy[index] = str[index];
    }

    return copy;
}

int stringToInt( const char *str ) 
{
    /**
     * Desc:
     *      Convert a NULL TERMINATED string to an integer.
     *
     * Args:
     *      str (const char*): String to convert. Must be null terminated.
     *
     * Returns:
     *      If str is a valid string, an integer representation of the string
     *      is returned. Otherwise, 0 is returned.
     */
    int integer = 0;
    int sign = 1;

    // Flip the sign if the number is supposed to be negative
    if( ( *str ) == ( '-' ) ) 
    {
        sign = -1;
        str++;
    } 
    else if( ( *str ) == ( '+' ) ) 
    {
        str++;
    }

    while( *str )
    {
        if( ( *str ) == ( ' ' ) ) 
        {
            str++;
            continue;
        }

        integer = ( integer * 10 ) + ( (*str) - '0' );
        str++;
    }

    return integer * sign;
}

double stringToDouble( const char *str )
{
    /**
     * Desc:
     *      Convert a string to a double.
     *
     * Args:
     *      str (cont char*): NULL TERMINATED string representation of a
     *                        double.
     *
     * Returns:
     *      If the string is a valid representation of a double, a double is
     *      returned. Otherwise, 0 is returned.
     */
    double leftDecimal = 0;
    double rightDecimal = 0;
    int sign = 1;
    int sigFig = 0;

    // Flip the sign if the number is supposed to be negative
    if( ( *str ) == ( '-' ) )
    {
        sign = -1;
        str++;
    } 
    else if( ( *str ) == ( '+' ) )
    {
        str++;
    }

    // Find the numerical representation of whatever is left of the decimal
    while( 1 )
    {
        // If we hit the end, there's no decimal places, so just return
        if( ( *str ) == ( ' ' ) ) 
        {
            str++;
            continue;
        }
        
        if( ( *str ) == ( '\0' ) ) 
        {
            return leftDecimal;
        }

        // If we hit the end of the left side, break
        if( ( *str ) == ( '.' ) )
        {
            str++;
            break;
        }

        // Build up the left number, same approach as stoi()
        leftDecimal = ( leftDecimal * 10 ) + ( *str - '0' );
        str++;
    }

    // Do the same for whatever is right of the decimal
    while( 1 )
    {
        if( ( *str ) == ( '\0' ) )
        {
            break;
        }
        
        // Build up left side, same approach as stoi()
        rightDecimal = ( rightDecimal * 10 ) + ( *str - '0' );
        str++;
        sigFig++;
    }

    // Now, combine the left and right side. Shift the right side into the
    // decimals
    return ( leftDecimal + ( rightDecimal / pow( 10, sigFig ) ) ) * sign;
}

void stripString( char *str ) 
{
    /**
     * Desc:
     *      Strip any newline characters and preceding spaces from a string by
     *      replacing newline characters with a null terminator.
     *
     * Args:
     *      str (char*): String (MUST BE NULL TERMINATED)
     */
    if( ( str[0] ) == ( ' ' ) )
    {
        for( int index = 0; ( str[index] ) != ( '\0' ); index++ )
        {
            str[index] = str[index + 1];
        }
    }

    int index = 0;
    while( ( str[index] ) != ( '\0' ) )
    {
        if( ( str[index] ) == ( '\n' ) )
        {
            str[index] = '\0';
            return;      
        }

        index++;
    }    
}

// End include guard
#endif              // STRINGUTILITIES_C
