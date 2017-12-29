/*
 * misc.h
 *
 *  Created on: 13.5.2010
 *      Author: hth
 */

#ifndef MISC_H_
#define MISC_H_

#include "../const.h"

#define PI 3.141593f
#define CLAMP(a, f0, f1) ( (a) < (f0) ? (f0) : ( (a) > (f1) ? (f1) : (a) ) )
#define INRANGE(a, f0, f1) ((a) >= (f0) && (a) <= (f1))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define LERP(a, b, t, t0) ((a) + ((b)-(a))*(t)/(t0))
#define RND ((float)(rand()%1000) / 1000) // Random number: [0, 1)
#define URND(a, b) ((a) + RND * ((b)-(a)))
#define ANG2RAD(a) (PI*(a)/180.f)

/*
 * f(x) = ax^2 + bx + c
 * f(0) = A => c = A
 * f'(T) = 0 => 2aT + b = 0 => b = -2aT
 * f(T) = B => aT^2 - 2aT^2 + A = B => -aT^2 + A = B => a = (B-A)/(-T^2) = (A-B)/T^2
 */


#endif /* MISC_H_ */
