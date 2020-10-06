#pragma once

#include <inttypes.h>
#include <assert.h>

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

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__)
#define FORCE_INLINE __attribute__((always_inline)) inline
#endif

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#ifdef _MSC_VER
#define PRINTF_FORMAT _Printf_format_string_
#else
#define PRINTF_FORMAT
#endif

template <class T, size_t N>
constexpr size_t ARRAYCOUNT(const T(&array)[N]) noexcept { return N; }

// NOTE: This is required for EASTL.
inline void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line){
	return new uint8_t[size];
}
inline void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
	//return new (std::align_val_t(alignment)) uint8_t[size];
	return operator new[](size, std::align_val_t(alignment));
}
