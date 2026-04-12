//! License: `GPL-3.0-or-later`.

#include "Common.h"
#if defined(CBUILD_API_STRICT_POSIX) || (defined(CBUILD_API_POSIX) && ( \
			(defined(CBUILD_OS_LINUX) && \
				!(defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL))) || \
			defined(CBUILD_OS_MACOS) || \
			defined(CBUILD_OS_BSD) || \
			defined(CBUILD_OS_WINDOWS_CYGWIN)))
	extern const char* __progname;
#endif // CBUILD_API_POSIX + ext check || CBUILD_API_STRICT_POSIX
CBUILDDEF const char* __cbuild_progname(void) {
	#if defined(CBUILD_API_POSIX)
		#if defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL) || \
			defined(CBUILD_OS_WINDOWS_CYGWIN)
			return program_invocation_short_name;
		#elif defined(CBUILD_OS_BSD) || defined(CBUILD_OS_MACOS)
			return getprogname();
		#else
			return __progname;
		#endif // CBUILD_OS_*
	#elif defined(CBUILD_API_STRICT_POSIX)
		return __progname;
	#endif // CBUILD_API_*
}
CBUILDDEF void* __cbuild_memrchr(const void* s, int c, size_t n) {
	#if defined(CBUILD_API_POSIX) && ( \
			defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL) || defined(CBUILD_OS_LINUX_UCLIBC) || \
			defined(CBUILD_OS_BSD))
		return memrchr(s, c, n);
	#else
		char* chrptr = (char*)s;
		chrptr += n;
		do {
			chrptr--;
			if(*chrptr == c) {
				goto loop_end;
			}
		} while(chrptr > (char*)s);
		chrptr = NULL;
	loop_end:
		return chrptr;
	#endif // Extension check
}
CBUILDDEF void* __cbuild_memmem(const void* haystack, size_t hsize,
	const void* needle, size_t nsize) {
	#if defined(CBUILD_API_POSIX) && ( \
			defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL) || defined(CBUILD_OS_LINNUX_UCLIBC) || \
			defined(CBUILD_OS_BSD) || \
			defined(CBUILD_OS_MACOS))
		return memmem(haystack, hsize, needle, nsize);
	#else
		if (hsize < nsize) return NULL;
		const char* orig = haystack;
		const char* p = orig;
		size_t rem = hsize;
		while ((p = memchr(p, *(const char*)needle, rem)) != NULL) {
			size_t offset = (size_t)(p - orig);
			rem = hsize - offset;
			if (rem < nsize) return NULL;
			if (memcmp(needle, p, nsize) == 0) return (void*)p;
			p++;
			rem--;
		}
		return NULL;
	#endif // Extension check
}
CBUILDDEF void __cbuild_assert(const char* file, unsigned int line, const char* func,
	const char* expr, ...) {
	__CBUILD_PRINTF("%s: %s:%u: %s: Assertion \"%s\" failed with message:\n",
		__cbuild_progname(), file, line, func, expr);
	va_list args;
	va_start(args, expr);
	const char* fmt = va_arg(args, char*);
	__CBUILD_VPRINTF(fmt, args);
	va_end(args);
	__CBUILD_FLUSH();
	abort();
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF uint64_t cbuild_time_nanos(void) {
		struct timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		return (uint64_t)(CBUILD_NANOS_PER_SEC * t.tv_sec + t.tv_nsec);
	}
#endif // CBUILD_API_*
