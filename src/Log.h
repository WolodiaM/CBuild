#pragma once // For LSP 
//! This file provides small logger utility.

#include "Common.h"

#ifndef CBUILD_LOG_CUSTOM_LEVELS
	/// Supported log levels. Compared arithmetically.
	///
	/// This can be overridden by defining `CBUILD_LOG_CUSTOM_LEVELS` and then providing
	/// enumeration `cbuild_custom_log_level_t` (without typedef).
	/// This 4 levels should be present in you implementation of this enum, because
	/// they are used by CBuild itself.
	///
	/// You would also need to provide `const char* __cbuild_int_log_level_names[]`.
	/// This array is used by default handler to format names of log levels.
	typedef enum {
		CBUILD_LOG_ERROR   = 1,
		CBUILD_LOG_WARN    = 2,
		CBUILD_LOG_INFO    = 3,
		CBUILD_LOG_TRACE   = 4,
	} cbuild_log_level_t;
#else
	typedef enum cbuild_custom_log_level_t cbuild_log_level_t;
#endif // CBUILD_LOG_CUSTOM_LEVELS

/// Main dispatcher for logger. Support printf-style formatting.
///
/// * [pl:level] Log level. Will be used both for printing and for disabling logs based on minimum level. 
/// * [pl:fmt] Format string for printf.
/// * [pl:...] Arguments for format string.
CBUILD_ATTR_PRINTF(2,
	CBUILDDEF void cbuild_log(cbuild_log_level_t level, const char* fmt, ...));

/// Wrapper for [`cbuild_log`](#cbuild_log) that sets level to `CBUILD_LOG_ERROR`.
#define cbuild_log_error(msg, ...)                            \
	cbuild_log(CBUILD_LOG_ERROR, msg __VA_OPT__(,) __VA_ARGS__)
/// Wrapper for [`cbuild_log`](#cbuild_log) that sets level to `CBUILD_LOG_WARN`.
#define cbuild_log_warn(msg, ...)                             \
	cbuild_log(CBUILD_LOG_WARN, msg __VA_OPT__(,) __VA_ARGS__)
/// Wrapper for [`cbuild_log`](#cbuild_log) that sets level to `CBUILD_LOG_INFO`.
#define cbuild_log_info(msg, ...)                             \
	cbuild_log(CBUILD_LOG_INFO, msg __VA_OPT__(,) __VA_ARGS__)
/// Wrapper for [`cbuild_log`](#cbuild_log) that sets level to `CBUILD_LOG_TRACE`.
#define cbuild_log_trace(msg, ...)                            \
	cbuild_log(CBUILD_LOG_TRACE, msg __VA_OPT__(,) __VA_ARGS__)

/// Same as [`cbuild_log`](#cbuild_log) but takes `va_list`{.c} instead of variadics. 
///
/// * [pl:level] Log level. Will be used both for printing and for disabling logs based on minimum level. 
/// * [pl:fmt] Format string for printf.
/// * [pl:args] Arguments for format string.
CBUILDDEF void cbuild_vlog(cbuild_log_level_t level, const char* fmt, va_list args);

/// Set minimum level of logs that will be printed.
///
/// Name of this functions is rather misleading, because it disable all levels that are
/// arithmetically higher than specified here. So technically it set maximum level. But
/// for logs levels lower technically means  less severe, so name is like this.
CBUILDDEF void cbuild_log_set_min_level(cbuild_log_level_t level);
/// Get minimum level of logs that would be printed.
CBUILDDEF cbuild_log_level_t cbuild_log_get_min_level(void);

/// This function will be called only for messages that should be printed.
/// It can be treated as a formatter for logged messages.
///
/// * [pl:level] Log level. Needed to pretty-print it.
/// * [pl:fmt] Format string for printf.
/// * [pl:args] Arguments for format string.
typedef void (*cbuild_log_handler_t)(cbuild_log_level_t level, const char* fmt, va_list args);
/// Setup handler for logger. Passing NULL will disable logs.
/// Default log handler will be setup by default.
CBUILDDEF void cbuild_log_set_handler(cbuild_log_handler_t handler);
/// Get current log handler.
CBUILDDEF cbuild_log_handler_t cbuild_log_get_handler(void);
