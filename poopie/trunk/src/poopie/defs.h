
#ifndef POOPIE_DEFS_H_
#define POOPIE_DEFS_H_

#pragma warning( disable: 4018 ) // signed/unsigned mismatch

#include <float.h>
#include "base/Logger.h"

// PLATFORM

#define POOP_WIN32

// FLAGS

#if defined _DEBUG && !defined POOP_DEBUG
#define POOP_DEBUG
#endif

// TYPEDEFS

typedef unsigned int uint;

typedef unsigned long long int uint64;
typedef signed long long int int64;

typedef unsigned int uint32;
typedef signed int int32;

typedef unsigned short uint16;
typedef signed short int16;

typedef unsigned char uint8;
typedef signed char int8;

typedef double float64;
typedef float float32;

// ASSERT/DEBUG
#include <Windows.h>

#ifdef POOP_DEBUG

#define POOP_ASSERT(...) do { if ( !(__VA_ARGS__) ) DebugBreak(); } while(false)
#define IF_POOP_DEBUG(...) __VA_ARGS__
#else
#define POOP_ASSERT(...)
#define IF_POOP_DEBUG(...)
#endif

#define POOP_TODO POOP_ASSERT(false)

// Error handling

#define POOP_ERROR(s) { MessageBox( NULL, s, "grr", 0 ); *((int*)0) = 0; }
#define POOP_WARNING(s) printf( "%s\n", String(s).getCStr() );
#define POOP_INTERNAL_LOG(s) Logger::log( s )
#define POOP_LOG(s) Logger::log( s )

// MISC MATH (todo: move elsewhere)

#define PI 3.1416f
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, a, b) MAX( MIN( x, MAX(a,b) ), MIN(a,b) )
#define SAT(a) CLAMP(a,1,0)
#define RND(a,b) ( ((b)-(a))*((float)(rand()%10000))/10000 + (a) )


#endif
