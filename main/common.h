#pragma once

#include <inttypes.h>
#include <assert.h>

// TODO: not sure whether I'm gonna get used to this...
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
// may revert to this
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#ifdef _MSC_VER
#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable: 4324) //warning C4324: structure was padded due to alignment specifier
#pragma warning(disable: 4505) // unreferenced local function has been removed
#pragma warning(disable: 4100) // unreferenced formal parameter
//#pragma warning(disable: 4091) // 'keyword' : ignored on left of 'type' when no variable is declared
//#pragma warning(disable: 6509) // warning c6509: Return used on precondition
//#pragma warning(disable: 4351) //warning C4351: new behavior: elements of array 'x' will be default initialized
#endif