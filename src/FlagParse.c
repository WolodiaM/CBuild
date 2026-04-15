//! License: `GPL-3.0-or-later`.

#include "FlagParse.h"
#include "Common.h"
#include "StringBuilder.h"
#include "DynArray.h"
#include "Log.h"
#include "FS.h"
struct __cbuild_flag_t {
	bool found;
	// uint8_t __padding[3];
	const char* option;
	struct cbuild_flag_new_opts_t spec;
	cbuild_arglist_t args;
};
struct __cbuild_flags_t {
	struct __cbuild_flag_t* data;
	size_t size;
	size_t capacity;
};
struct __cbuild_flag_context_t {
	bool pass_separator;
	// uint8_t __padding[3];
	const char* app_name;
	cbuild_flag_print_func_t help;
	cbuild_flag_print_func_t version;
	cbuild_arglist_t pargs;
	struct __cbuild_flags_t flags;
};
struct __cbuild_flag_context_t __cbuild_flag_context;
CBUILDDEF void cbuild_flag_new_opt(const char* option, struct cbuild_flag_new_opts_t opts) {
	struct __cbuild_flag_t flag = {0};
	flag.option = option;
	flag.spec = opts;
	cbuild_da_append(&__cbuild_flag_context.flags, flag);
}
CBUILDDEF void cbuild_flag_set_option(enum cbuild_flag_options_t option, ...) {
	va_list args;
	va_start(args, option);
	switch (option) {
	case CBUILD_FLAG_PASS_SEPARATOR: {
		__cbuild_flag_context.pass_separator = true;
	} break;
	case CBUILD_FLAG_HELP_HOOK: {
		cbuild_flag_print_func_t f = va_arg(args,cbuild_flag_print_func_t);
		__cbuild_flag_context.help = f;
	} break;
	case CBUILD_FLAG_VERSION_HOOK: {
		cbuild_flag_print_func_t f = va_arg(args,cbuild_flag_print_func_t);
		__cbuild_flag_context.version = f;
	} break;
	}
	va_end(args);
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF unsigned short __cbuild_flag_term_width(void) {
		#if defined(TIOCGWINSZ)
			cbuild_fd_t fd = cbuild_fd_open_read("/dev/tty");
			if (fd == CBUILD_INVALID_FD) return 80;
			struct winsize w;
			if (ioctl(fd, TIOCGWINSZ, &w) == -1) {
				cbuild_fd_close(fd);
				return 80;
			}
			cbuild_fd_close(fd);
			return w.ws_col;
		#else
			return 80;
		#endif // Extension check
	}
	void __cbuild_flag_term_get_cursor(int* x, int* y) {
		struct termios orig = {0};
		tcgetattr(STDIN_FILENO, &orig);
		struct termios new = orig;
		new.c_lflag &= ~((unsigned int)ICANON | (unsigned int)ECHO);
		new.c_cc[VMIN] = 1;
		new.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &new);
		const char req[] = "\e[6n";
		write(STDOUT_FILENO, req, sizeof(req));
		char resp[9] = {0}; 
		cbuild_assert(read(STDIN_FILENO, resp, 8) >= 0,
			"Can not read from stdin.\n");
		sscanf(resp, "\033[%d;%dR", x, y);
		tcsetattr(STDIN_FILENO, TCSANOW, &orig);
	}
#endif // CBUILD_API_*
CBUILDDEF void __cbuild_flag_help(const char* name) {
	__CBUILD_PRINTF("Usage: %s [OPTIONS]\n\n", name);
	cbuild_flag_print_help();
}
CBUILDDEF void __cbuild_flag_version(const char* name) {
	__CBUILD_PRINTF("%s - v1.0\n", name);
}
CBUILDDEF struct __cbuild_flag_t* __cbuild_flag_lookup_flag(const char* name) {
	cbuild_da_foreach(__cbuild_flag_context.flags, flag) {
		if (strcmp(flag->option, name) == 0) {
			return flag;
		}
	}
	return NULL;
}
CBUILDDEF struct __cbuild_flag_t* __cbuild_flag_lookup_sflag(char name) {
	cbuild_da_foreach(__cbuild_flag_context.flags, flag) {
		if (flag->spec.short_option == name) {
			return flag;
		}
	}
	return NULL;
}
CBUILDDEF void __cbuild_flag_parse_arguments(struct __cbuild_flag_t* flag,
	char** argv, int argc, int* parser) {
	// NOTE: This technically allows TList to terminate on one condition besides
	// terminator - end of flags stream. This should not cause too much problems.
	int found = 0;
	while (argc > *parser) {
		char* arg = NULL;
		if (flag->spec.terminator) {
			arg = argv[(*parser)++];
			if (strcmp(arg, flag->spec.terminator) == 0) break;
		} else {
			if (found == flag->spec.num_arguments) break;
			arg = argv[(*parser)++];
			if (arg[0] == '-') {
				(*parser)--;
				break;
			}
		}
		cbuild_da_append(&flag->args, arg);
		found++;
	}
	// Arguments are options and none were found
	if (flag->spec.optional && found == 0) return;
	// Any number of arguments are fine
	if (flag->spec.num_arguments == -1) return;
	// Enough arguments found - this is fine
	if (found == flag->spec.num_arguments) return;
	// More arguments found - can be triggered only for a TList
	if (found > flag->spec.num_arguments) {
		cbuild_log_error(
			
			"Too many arguments for flag --%s, %d provided but %d expected.",
			flag->option, found, flag->spec.num_arguments);
		__cbuild_flag_context.help(__cbuild_flag_context.app_name);
		exit(1);
	}
	// Less arguments found
	if (found < flag->spec.num_arguments) {
		cbuild_log_error(
			
			"Too few arguments for flag --%s, %d provided but %d expected.",
			flag->option, found, flag->spec.num_arguments);
		__cbuild_flag_context.help(__cbuild_flag_context.app_name);
		exit(1);
	}
}
CBUILDDEF void cbuild_flag_parse(int argc, char** argv) {
	// Register built-in flags and parameters
	cbuild_flag_new("help", .short_option = 'h', .desc = "Print this message.");
	cbuild_flag_new("version", .short_option = 'v',
		.desc = "Print version information.");
	if (__cbuild_flag_context.help == NULL) {
		cbuild_flag_set_option(CBUILD_FLAG_HELP_HOOK, __cbuild_flag_help);
	}
	if (__cbuild_flag_context.version == NULL) {
		cbuild_flag_set_option(CBUILD_FLAG_VERSION_HOOK, __cbuild_flag_version);
	}
	// Parse flags
	__cbuild_flag_context.app_name = argv[0];
	bool parse_flags = true;
	int parser = 1;
	while (parser < argc) {
		char* arg = argv[parser++];
		if (!parse_flags) { // Parser is inhibited
			cbuild_da_append(&__cbuild_flag_context.pargs, arg);
		} else if (strcmp(arg, "--") == 0) { // Special case - '--'
			parse_flags = false;
			if (__cbuild_flag_context.pass_separator) {
				cbuild_da_append(&__cbuild_flag_context.pargs, arg);
			}
		} else if (arg[0] == '-' && arg[1] == '-') { // Long option flag
			arg += 2;
			char* equal = strchr(arg, '=');
			if (equal != NULL) {
				*equal = '\0';
				struct __cbuild_flag_t* flag = __cbuild_flag_lookup_flag(arg);
				if (flag == NULL) {
					cbuild_log_error("Invalid flag specified: --%s", arg);
					__cbuild_flag_context.help(__cbuild_flag_context.app_name);
					exit(1);
					continue;
				}
				if (flag->found && !flag->spec.repeat) {
					cbuild_log_error("Flag --%s does not support being repeated.", arg);
					__cbuild_flag_context.help(__cbuild_flag_context.app_name);
					exit(1);
				}
				flag->found = true;
				if (flag->spec.terminator != NULL) {
					cbuild_log_error(
						"Terminated list can not take arguments with \"--%s=argument\".", arg);
					__cbuild_flag_context.help(__cbuild_flag_context.app_name);
					exit(1);
				} else if (flag->spec.num_arguments == 0) {
					cbuild_log_error(
						"Too many arguments for flag --%s, 1 provided but %d expected.",
						arg, flag->spec.num_arguments);
					__cbuild_flag_context.help(__cbuild_flag_context.app_name);
					exit(1);
				} else if (flag->spec.num_arguments > 1) {
					cbuild_log_error(
						"Too few arguments for flag --%s, 1 provided but %d expected.",
						arg, flag->spec.num_arguments);
					__cbuild_flag_context.help(__cbuild_flag_context.app_name);
					exit(1);
				}
				cbuild_da_append(&flag->args, equal + 1);
				*equal = '=';
			} else {
				struct __cbuild_flag_t* flag = __cbuild_flag_lookup_flag(arg);
				if (flag == NULL) {
					cbuild_log_error("Invalid flag specified: --%s.", arg);
					__cbuild_flag_context.help(__cbuild_flag_context.app_name);
					exit(1);
					continue;
				}
				if (flag->found && !flag->spec.repeat) {
					cbuild_log_error("Flag --%s does not support being repeated.", arg);
					__cbuild_flag_context.help(__cbuild_flag_context.app_name);
					exit(1);
				}
				flag->found = true;
				__cbuild_flag_parse_arguments(flag, argv, argc, &parser);
			}
		} else if (arg[0] == '-') { // Short option flag
			arg += 1;
			char* equal = strchr(arg, '=');
			if (equal != NULL) *equal = '\0';
			size_t len = strlen(arg);
			for (size_t i = 0; i < len; i++) {
				struct __cbuild_flag_t* flag = __cbuild_flag_lookup_sflag(arg[i]);
				if (flag == NULL) {
					cbuild_log_error("Invalid flag specified: -%c.", arg[i]);
					__cbuild_flag_context.help(__cbuild_flag_context.app_name);
					exit(1);
					continue;
				}
				if (flag->found && !flag->spec.repeat) {
					cbuild_log_error("Flag -%c does not support being repeated.", arg[i]);
					__cbuild_flag_context.help(__cbuild_flag_context.app_name);
					exit(1);
				}
				flag->found = true;
				if ((i + 1) == len) {
					if (equal != NULL) {
						if (flag->spec.terminator != NULL) {
							cbuild_log_error(
								"Terminated list can not take arguments with \"-%c=argument\".", arg[i]);
							__cbuild_flag_context.help(__cbuild_flag_context.app_name);
							exit(1);
						} else if (flag->spec.num_arguments == 0) {
							cbuild_log_error(
								"Too many arguments for flag -%c, 1 provided but %d expected.",
								arg[i], flag->spec.num_arguments);
							__cbuild_flag_context.help(__cbuild_flag_context.app_name);
							exit(1);
						} else if (flag->spec.num_arguments > 1) {
							cbuild_log_error(
								"Too few arguments for flag -%c, 1 provided but %d expected.",
								arg[i], flag->spec.num_arguments);
							__cbuild_flag_context.help(__cbuild_flag_context.app_name);
							exit(1);
						}
						cbuild_da_append(&flag->args, equal + 1);
					} else {
						__cbuild_flag_parse_arguments(flag, argv, argc, &parser);
					}
				} else {
					if (flag->spec.num_arguments != 0 && !flag->spec.optional) {
						cbuild_log_error(
							"Too few arguments for flag -%c, 1 provided but %d expected.",
							arg[i], flag->spec.num_arguments);
						__cbuild_flag_context.help(__cbuild_flag_context.app_name);
						exit(1);
					}
				}
			}
			if (equal != NULL) *equal = '=';
		} else { // Positional argument
			cbuild_da_append(&__cbuild_flag_context.pargs, arg);
		}
	}
	// Check built-in flags
	if (cbuild_flag_get_flag("help")) {
		__cbuild_flag_context.help(__cbuild_flag_context.app_name);
		exit(0);
	}
	if (cbuild_flag_get_flag("version")) {
		__cbuild_flag_context.version(__cbuild_flag_context.app_name);
		exit(0);
	}
}
// 'len' is length of column with flag values, 30 max (if flag is longer than it should just wrap first line a little differently, no need to penalize all flags.
CBUILDDEF char* __cbuild_flag_fmt_flag(struct __cbuild_flag_t* flag, int len) {
	cbuild_sb_t sb = {0};
	cbuild_sb_append_cstr(&sb, "  ");
	if (flag->spec.short_option != '\0') {
		cbuild_sb_appendf(&sb, "-%c, ", flag->spec.short_option);
	}
	cbuild_sb_appendf(&sb, "--%s", flag->option);
	if (flag->spec.num_arguments) {
		cbuild_sb_append(&sb, ' ');
		if (flag->spec.optional) cbuild_sb_append(&sb, '[');
		else cbuild_sb_append(&sb, '<');
		if (flag->spec.argument_desc) cbuild_sb_append_cstr(&sb, flag->spec.argument_desc);
		else cbuild_sb_append_cstr(&sb, "ARGUMENT");
		if (flag->spec.terminator) {
			cbuild_sb_appendf(&sb, "|%s", flag->spec.terminator);
		}
		if (flag->spec.optional) cbuild_sb_append(&sb, ']');
		else cbuild_sb_append(&sb, '>');
	}
	for (int i = (int)sb.size; i < len; i++) cbuild_sb_append(&sb, ' ');
	cbuild_sb_append(&sb, ' ');
	size_t col1w = sb.size;
	ssize_t desc_len = (ssize_t)strlen(flag->spec.desc);
	const char* desc = flag->spec.desc;
	const size_t line_len = __cbuild_flag_term_width() - col1w;
	while (desc_len > 0) {
		if (desc != flag->spec.desc) { // This is means this is not a first iteration
			for (size_t i = 0; i < col1w; i++) cbuild_sb_append(&sb, ' ');
		}
		cbuild_sb_append_arr(&sb, desc, CBUILD_MIN(line_len, (size_t)desc_len));
		cbuild_sb_append(&sb, '\n');
		desc += line_len;
		desc_len -= (ssize_t)line_len;
	}
	cbuild_sb_append_null(&sb);
	return sb.data;
}
CBUILDDEF void cbuild_flag_print_help(void) {
	// Measure size of flag name column
	int name_len = 0;
	cbuild_da_foreach(__cbuild_flag_context.flags, flag) {
		int flag_len = 3; // '--' and space after it.
		if (flag->spec.short_option != '\0') flag_len += 4;
		flag_len += (int)strlen(flag->option);
		if (flag->spec.num_arguments != 0) {
			flag_len += 3; // ' <...>' or ' [...]'
			if (flag->spec.argument_desc) flag_len += (int)strlen(flag->spec.argument_desc);
			else flag_len += 8;
			if (flag->spec.terminator != NULL) {
				flag_len += 1; // 'Terminator is shown as 'desc|terminator'
				flag_len += (int)strlen(flag->spec.terminator);
			}
		} else {
			flag_len += 1;
		}
		name_len = CBUILD_MAX(name_len, flag_len);
	}
	name_len = CBUILD_MIN(name_len, 30); // 30 is just arbitrary number.
	// Print ungrouped arguments and collect list of groups
	cbuild_arglist_t groups = {0};
	__CBUILD_PRINT("Flags:\n");
	cbuild_da_foreach(__cbuild_flag_context.flags, flag) {
		if (flag->spec.group == NULL) {
			char* desc = __cbuild_flag_fmt_flag(flag, name_len);
			__CBUILD_PRINT(desc);
			__CBUILD_FREE(desc);
		} else {
			bool group_found = false;
			cbuild_da_foreach(groups, group) {
				if (strcmp(*group, flag->spec.group) == 0) {
					group_found = true;
					break;
				}
			}
			if (!group_found) cbuild_da_append(&groups, (char*)flag->spec.group);
		}
	}
	// Print all grouped flags
	cbuild_da_foreach(groups, group) {
		__CBUILD_PRINTF("%s\n", *group);
		cbuild_da_foreach(__cbuild_flag_context.flags, flag) {
			if (flag->spec.group) {
				if (strcmp(flag->spec.group, *group) == 0) {
					char* desc = __cbuild_flag_fmt_flag(flag, name_len);
					__CBUILD_PRINT(desc);
					__CBUILD_FREE(desc);
				}
			}
		}
	}
	cbuild_da_clear(&groups);
}
CBUILDDEF cbuild_arglist_t* cbuild_flag_get_pargs(void) {
	return &__cbuild_flag_context.pargs;
}
CBUILDDEF cbuild_arglist_t* cbuild_flag_get_flag(const char* opt) {
	struct __cbuild_flag_t* flag = __cbuild_flag_lookup_flag(opt);
	if (flag == NULL) return NULL;
	if (!flag->found) return NULL;
	return &flag->args;
}
CBUILDDEF const char* cbuild_flag_app_name(void) {
	return __cbuild_flag_context.app_name;
}
