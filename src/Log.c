//! License: `GPL-3.0-or-later`.

#include "Log.h"
#include "Common.h"
#include "Term.h"
// Internals
CBUILDDEF void __cbuild_default_log_handler(const char* level, const char* fmt, 
	va_list args) {
	time_t t = time(NULL);
	struct tm* tm_info = localtime(&t);
	__CBUILD_PRINTF("[%02d:%02d:%02d] ", tm_info->tm_hour, tm_info->tm_min,
		tm_info->tm_sec);
	__CBUILD_PRINT(level);
	__CBUILD_VPRINTF(fmt, args);
	__CBUILD_PRINT("\n");
}
cbuild_log_handler_t __cbuild_curr_log_handler = __cbuild_default_log_handler;
cbuild_log_level_t __cbuild_min_log_level = CBUILD_LOG_MIN_LEVEL;
#ifndef CBUILD_LOG_CUSTOM_LEVELS
	const char* __cbuild_log_level_names[] = {
		[CBUILD_LOG_ERROR] = CBUILD_TERM_FG(CBUILD_TERM_RED)"[ERROR]"CBUILD_TERM_RESET" ",
		[CBUILD_LOG_WARN]  = CBUILD_TERM_FG(CBUILD_TERM_YELLOW)"[WARN]"CBUILD_TERM_RESET" ",
		[CBUILD_LOG_INFO]  = "[INFO] ",
		[CBUILD_LOG_TRACE] = CBUILD_TERM_FG(CBUILD_TERM_BRBLACK)"[TRACE]"CBUILD_TERM_RESET" ",
	};
#else
	extern const char* __cbuild_log_level_names[];
#endif // CBUILD_LOG_CUSTOM_LEVELS
// API
CBUILDDEF void cbuild_log(cbuild_log_level_t level, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	cbuild_vlog(level, fmt, args);
	va_end(args);
}
void cbuild_vlog(cbuild_log_level_t level, const char* fmt, va_list args) {
	if (level <= __cbuild_min_log_level) {
		if(__cbuild_curr_log_handler) {
			__cbuild_curr_log_handler(__cbuild_log_level_names[level], fmt, args);
		}
	}
}
CBUILDDEF void cbuild_log_set_min_level(cbuild_log_level_t level) {
	__cbuild_min_log_level = level;
}
CBUILDDEF cbuild_log_level_t cbuild_log_get_min_level(void) {
	return __cbuild_min_log_level;
}
CBUILDDEF void cbuild_log_set_handler(cbuild_log_handler_t handler) {
	__cbuild_curr_log_handler = handler;
}
CBUILDDEF cbuild_log_handler_t cbuild_log_get_handler(void) {
	return __cbuild_curr_log_handler;
}
