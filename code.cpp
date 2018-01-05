/*
 * code.cpp
 *
 *  Created on: Apr 23, 2009
 *      Author: KFodor
 */

#include <stdint.h>
#include "code.h"

#define DIM(x) (sizeof((x)) / sizeof((x)[0]))

Code::Code( void ) // ctor
{
    clear();
    return;
}

void Code::clear (void)
{
    // Initialize index pointer to where to place the next
    // incoming digit (resets the code).
    index = 0;

    return;
}

bool Code::good( void )
{
    // Enough digits entered?
    return (index == DIM(code));
}

Code& Code::operator+=(char key)
{
    // Make sure the input is in fact something we care about (a valid digit).
    if((key >= '0') && (key <= '9'))
    {
        // Check if our capacity for digits is reached, if so we begin
        // shifting in new digits from the right to left.
        if(index == DIM(code))
        {
            // Shift code left one position
            for(uint8_t i=0; i < (DIM(code) - 1); i++)
            {
                code[i] = code[i+1];
            }

            // Add new key to the last position
            code[DIM(code) - 1] = key;
        }
        else
        {
            // Add key in current position and increment to the next position
            // from left to right.
            code[index++] = key;
        }
    }

    return *this;
}

int Code::operator==(const Code& right)
{
    // Is the size the same? If sizes are not the same they cannot
    // be equal.
    int result = (index == right.index);
    if(result)
    {
        // check each digit for equality. If any one digit is != then
        // we stop checking and return false.
        for(uint8_t i=0; i < DIM(code); i++)
        {
            // Compare digit
            result = (code[i] == right.code[i]);
            if(!result) break;
        }
    }

    return result;
}
