#pragma once

#include <inttypes.h>
#include <cstddef>
#include <assert.h>

using int8   = int8_t;
using int16  = int16_t;
using int32  = int32_t;
using int64  = int64_t;
using uint8  = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

#ifdef _MSC_VER
#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable: 4324) //warning C4324: structure was padded due to alignment specifier
#pragma warning(disable: 4505) // unreferenced local function has been removed
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 28251) // Inconsistent annotation
//#pragma warning(disable: 4351) //warning C4351: new behavior: elements of array 'x' will be default initialized
#endif

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__)
#define FORCE_INLINE __attribute__((always_inline)) inline
#endif

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

// TODO: gcc/clang attributes work very differently here, but I haven't yet figured out how to make a "declare printf-like function" macro with arbitrary arguments
#ifdef _MSC_VER
#define PRINTF_FORMAT _Printf_format_string_
#else
#define PRINTF_FORMAT
#endif

template <class T, size_t N>
constexpr size_t ARRAYCOUNT(const T(&array)[N]) noexcept { return N; }

// NOTE: This is required for EASTL.
inline void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line){
	return operator new[](size);
}
inline void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
	return operator new[](size, std::align_val_t(alignment));
}

// TODO: Add string_view variants *somewhere* that are guaranteed to be null terminated (asserted in debug builds)?
