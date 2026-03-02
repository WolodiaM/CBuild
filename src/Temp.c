#include "Temp.h"
#include "Arena.h"
#include "Common.h"
cbuild_arena_t __cbuild_temp_arena = {0};
#define INIT()                                                               \
	do {                                                                       \
		if (!__cbuild_temp_arena.base) {                                         \
			cbuild_arena_base_alloc(&__cbuild_temp_arena, CBUILD_TEMP_ARENA_SIZE); \
		}                                                                        \
	} while(0)
CBUILDDEF void* cbuild_temp_malloc(size_t size) {
	INIT();
	return cbuild_arena_malloc(&__cbuild_temp_arena, size);
}
CBUILDDEF void* cbuild_temp_realloc(void* ptr, size_t size) {
	INIT();
	return cbuild_arena_realloc(&__cbuild_temp_arena, ptr, size);
}
CBUILDDEF char* cbuild_temp_sprintf(const char* fmt, ...) {
	INIT();
	va_list va;
	va_start(va, fmt);
	char* ret = cbuild_arena_vsprintf(&__cbuild_temp_arena, fmt, va);
	va_end(va);
	return ret;
}
CBUILDDEF char* cbuild_temp_vsprintf(const char* fmt, va_list ap) {
	INIT();
	return cbuild_arena_vsprintf(&__cbuild_temp_arena, fmt, ap);
}
CBUILDDEF char* cbuild_temp_strdup(const char* str) {
	INIT();
	return cbuild_arena_strdup(&__cbuild_temp_arena, str);
}
CBUILDDEF void* cbuild_temp_memdup(const void* mem, size_t size) {
	INIT();
	return cbuild_arena_memdup(&__cbuild_temp_arena, mem, size);
}
CBUILDDEF size_t cbuild_temp_checkpoint(void) {
	INIT();
	return cbuild_arena_checkpoint(&__cbuild_temp_arena);
}
CBUILDDEF void cbuild_temp_reset(size_t checkpoint) {
	INIT();
	cbuild_arena_reset(&__cbuild_temp_arena, checkpoint);
}
#undef INIT
