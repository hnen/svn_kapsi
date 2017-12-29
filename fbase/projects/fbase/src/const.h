/*
 * const.h
 *
 *  Created on: 4.7.2010
 *      Author: hth
 */

#ifndef CONST_H_
#define CONST_H_

typedef unsigned int uint;
typedef unsigned char byte;

typedef unsigned long long int uint64;
typedef long long int int64;

#include "FObj.h"

#define FCLASS(T) class C ## T; typedef FObjPtr< C ## T > T; class C ## T

#endif /* CONST_H_ */
