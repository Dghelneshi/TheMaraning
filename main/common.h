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

#define TM_CONCAT_IMPL(x, y) x##y
#define TM_CONCAT(x, y) TM_CONCAT_IMPL(x, y)

#define TM_STRINGIFY_IMPL(x) #x
#define TM_STRINGIFY(x) TM_STRINGIFY_IMPL(x)

#ifdef _MSC_VER
#define TM_INLINE __forceinline
#elif defined(__GNUC__)
#define TM_INLINE __attribute__((always_inline)) inline
#endif

#ifdef _MSC_VER
#define TM_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define TM_NOINLINE __attribute__((noinline))
#endif

#ifdef _MSC_VER
#define TM_DEBUGBREAK __debugbreak()
#elif defined(__clang__)
#define TM_DEBUGBREAK __builtin_debugtrap()
#else
#define TM_DEBUGBREAK { __asm int 3 }
#endif

#ifdef NDEBUG
	#ifdef _MSC_VER
		#define TM_ASSUME(x) __assume(!!(x))
	#elif defined(__clang__)
		#define TM_ASSUME(x) __builtin_assume(!!(x))
	#else // GCC has __builtin_unreachable() but it may not optimize away side effects in the same way as __builtin_assume()
		#define TM_ASSUME(x) (void)(x)
	#endif
#else
	#define TM_ASSUME(x) TM_ASSERT(x)
#endif

#ifdef NDEBUG
#define TM_ASSERT(x) TM_ASSUME(x)
#else
#define TM_ASSERT(x) assert(x)
#endif

#ifdef NDEBUG
#define TM_RELEASE_ASSERT(x) do { if (!(x)) TM_DEBUGBREAK; } while(0)
#else
#define TM_RELEASE_ASSERT(x) assert(x)
#endif

#ifdef NDEBUG
	#ifdef _MSC_VER
		#define TM_UNREACHABLE __assume(false)
	#elif defined(__GNUC__)
		#define TM_UNREACHABLE __builtin_unreachable()
	#endif
#else
	#define TM_UNREACHABLE do { TM_ASSERT(false); std::terminate(); } while(0)
#endif

template <class T, size_t N>
constexpr size_t ARRAYCOUNT(const T(&array)[N]) noexcept { return N; }

// NOTE: This is required for EASTL.
inline void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line) {
	return operator new[](size);
}
inline void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
	return operator new[](size, std::align_val_t(alignment));
}

// TODO: Add string_view variants *somewhere* that are guaranteed to be null terminated (asserted in debug builds)?
