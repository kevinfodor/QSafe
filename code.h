/*
 * code.h
 *
 *  Created on: Apr 23, 2009
 *      Author: KFodor
 */

#include <stdint.h>

#ifndef CODE_H_
#define CODE_H_

class Code
{

public:
    Code(); //ctor

    void clear(void);
    bool good(void);

    // Operators
    Code& operator+=(char key);
    int operator==(const Code& left);

private:
    // local attributes
    char code[4];
    uint8_t index;

};

#endif /* CODE_H_ */
