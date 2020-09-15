
#ifdef __cplusplus

#include <new>
#include "rpmalloc.h"

// NOTE(DGH): added [[nodiscard]] to new operators... doesn't seem to work.

// NOTE(DGH): option to disable all custom allocators for AddressSanitizer to work
#ifndef DISABLE_CUSTOM_ALLOCATORS

#ifdef _WIN32

extern void __CRTDECL
operator delete(void* p) noexcept {
	rpfree(p);
}

extern void __CRTDECL
operator delete[](void* p) noexcept {
	rpfree(p);
}

[[nodiscard]] extern void* __CRTDECL
operator new(std::size_t size) noexcept(false) {
	return rpmalloc(size);
}

[[nodiscard]] extern void* __CRTDECL
operator new[](std::size_t size) noexcept(false) {
	return rpmalloc(size);
}

[[nodiscard]] extern void* __CRTDECL
operator new(std::size_t size, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpmalloc(size);
}

[[nodiscard]] extern void* __CRTDECL
operator new[](std::size_t size, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpmalloc(size);
}

#if (__cplusplus >= 201402L || _MSC_VER >= 1916)

extern void __CRTDECL
operator delete(void* p, std::size_t size) noexcept {
	(void)sizeof(size);
	rpfree(p);
}

extern void __CRTDECL
operator delete[](void* p, std::size_t size) noexcept {
	(void)sizeof(size);
	rpfree(p);
}

#endif

#if (__cplusplus > 201402L || defined(__cpp_aligned_new))

extern void __CRTDECL
operator delete(void* p, std::align_val_t align) noexcept {
	(void)sizeof(align);
	rpfree(p);
}

extern void __CRTDECL
operator delete[](void* p, std::align_val_t align) noexcept {
	(void)sizeof(align);
	rpfree(p);
}

extern void __CRTDECL
operator delete(void* p, std::size_t size, std::align_val_t align) noexcept {
	(void)sizeof(size);
	(void)sizeof(align);
	rpfree(p);
}

extern void __CRTDECL
operator delete[](void* p, std::size_t size, std::align_val_t align) noexcept {
	(void)sizeof(size);
	(void)sizeof(align);
	rpfree(p);
}

[[nodiscard]] extern void* __CRTDECL
operator new(std::size_t size, std::align_val_t align) noexcept(false) {
	return rpaligned_alloc((size_t)align, size);
}

[[nodiscard]] extern void* __CRTDECL
operator new[](std::size_t size, std::align_val_t align) noexcept(false) {
	return rpaligned_alloc((size_t)align, size);
}

[[nodiscard]] extern void* __CRTDECL
operator new(std::size_t size, std::align_val_t align, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpaligned_alloc((size_t)align, size);
}

[[nodiscard]] extern void* __CRTDECL
operator new[](std::size_t size, std::align_val_t align, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpaligned_alloc((size_t)align, size);
}

#endif

#endif

#endif

#endif