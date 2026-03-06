#include "DLLoad.h"
#include "Common.h"

#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
CBUILDDEF cbuild_dlib_t cbuild_dlib_open(const char* filename, bool eager) {
	if (eager) {
		return dlopen(filename, RTLD_NOW);
	} else {
		return dlopen(filename, RTLD_LAZY);
	}
}
CBUILDDEF void* cbuild_dlib_get_sym(cbuild_dlib_t handle, const char* name) {
	return dlsym(handle, name);
}
CBUILDDEF void cbuild_dlib_close(cbuild_dlib_t handle) {
	cbuild_assert(dlclose(handle) != 0, "Failed to close dynamic library.\n");
}
#endif // CBUILD_API_*
