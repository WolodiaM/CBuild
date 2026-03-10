#include "FlagParse.h"
#include "Common.h"
#include "StringBuilder.h"
#include "DynArray.h"
#include "Log.h"
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
		struct winsize w;
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
			cbuild_log_error("Could not get terminal width. Falling back to default.");
			return 90;
		}
		return w.ws_col;
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
CBUILDDEF void __cbuild_flag_parse_args(struct __cbuild_flag_t* spec,
	char** argv, int* argc) {
	int num_parsed = 0;
	while (*argc > 0) {
		if (spec->spec.num_arguments == num_parsed) return; // For '-1' this will limit at ~2^32
		char* arg = cbuild_shift(argv, *argc);
		if (spec->spec.terminator == NULL && arg[0] == '-') {
			argv--;
			(*argc)++;
			break;
		} else if (spec->spec.terminator != NULL) {
			if (strcmp(spec->spec.terminator, arg) == 0) {
				argv--;
				(*argc)++;
				break;
			}
		}
		num_parsed++;
		cbuild_da_append(&spec->args, arg);
	}
	if (spec->spec.optional_arg && num_parsed == 0) {
		// Fine - args can be omitted and none were provided
	} else if (spec->spec.optional_arg && spec->spec.num_arguments == -1) {
		// Fine, anything goes
	} else if (spec->spec.num_arguments == -1 && num_parsed > 0) {
		// Fine, at least one argument needed 
	} else if (spec->spec.num_arguments == -1 && num_parsed == 0) {
		// Error, at least one arguments is needed
		cbuild_log_error(
			"Too little arguments provided for flag --%s, %d provided but some are expected",
			spec->option, num_parsed);
		__cbuild_flag_context.help(__cbuild_flag_context.app_name);
		exit(1);
	} else if (spec->spec.num_arguments == num_parsed) {
		// Fine, we have required number of arguments, optional does not matter here
	} else if (spec->spec.num_arguments != num_parsed) {
		// We parsed some arguments, but less than needed, this is an error, optional does not matter here
		cbuild_log_error(
			"Too little arguments provided for flag --%s, %d provided but %d expected",
			spec->option, num_parsed, spec->spec.num_arguments);
		__cbuild_flag_context.help(__cbuild_flag_context.app_name);
		exit(1);
	}
}
CBUILDDEF void __cbuild_flag_help(const char* name) {
	__CBUILD_PRINTF("Usage: %s [OPTIONS]\n\n", name);
	cbuild_flag_print_help();
}
CBUILDDEF void __cbuild_flag_version(const char* name) {
	__CBUILD_PRINTF("%s - v1.0\n", name);
}
CBUILDDEF void cbuild_flag_parse(int argc, char** argv) {
	// Register built-in flags and parameters
	cbuild_flag_new("help", .short_option = 'h', .desc = "Print this message.");
	cbuild_flag_new("version", .short_option = 'v',
		.desc = "Print version information.");
	cbuild_flag_set_option(CBUILD_FLAG_HELP_HOOK, __cbuild_flag_help);
	cbuild_flag_set_option(CBUILD_FLAG_VERSION_HOOK, __cbuild_flag_version);
	// Parse flags
	__cbuild_flag_context.app_name = cbuild_shift(argv, argc);
	bool parse_flags = true;
	while (argc > 0) {
		char* token = cbuild_shift(argv, argc);
		if (!parse_flags) { // Parser is inhibited
			cbuild_da_append(&__cbuild_flag_context.pargs, token);
		} else if (strcmp(token, "--") == 0) { // Special case - '--'
			parse_flags = false;
			if (__cbuild_flag_context.pass_separator) {
				cbuild_da_append(&__cbuild_flag_context.pargs, token);
			}
		} else if (token[0] == '-' && token[1] == '-') { // Long option flag
			cbuild_sv_t tk = cbuild_sv_from_cstr(token + 2); // Strip leading '--'
			cbuild_sv_t arg = {0};
			if (cbuild_sv_contains(tk, '=')) {
				arg = tk;
				tk = cbuild_sv_chop_by_delim(&arg, '=');
			}
			cbuild_da_foreach(__cbuild_flag_context.flags, spec) {
				if (cbuild_sv_cmp(tk, cbuild_sv_from_cstr(spec->option))) {
					spec->found = true;
					if (arg.data != NULL) {
						if (spec->spec.num_arguments == 1 || spec->spec.num_arguments == -1) {
							cbuild_da_append(&spec->args, arg.data); // 'arg.data' will be NULL-terminated
						} else if (spec->spec.num_arguments > 1) {
							cbuild_log_error(
								"Too few arguments for flag --"CBuildSVFmt", 1 provided but %d expected.",
								CBuildSVArg(tk), spec->spec.num_arguments);
							__cbuild_flag_context.help(__cbuild_flag_context.app_name);
							exit(1);
						} else if (spec->spec.num_arguments == 0) {
							cbuild_log_error(
								"Too many arguments for flag --"CBuildSVFmt", 1 provided but %d expected.",
								CBuildSVArg(tk), spec->spec.num_arguments);
							__cbuild_flag_context.help(__cbuild_flag_context.app_name);
							exit(1);
						}
					} else {
						__cbuild_flag_parse_args(spec, argv, &argc);
					}
					continue;
				}
			}
			cbuild_log_error("Invalid flag specified: --"CBuildSVFmt, CBuildSVArg(tk));
			__cbuild_flag_context.help(__cbuild_flag_context.app_name);
			exit(1);
		} else if (token[0] == '-') { // Short option flag
			token += 1; // Strip '-'
			size_t len = strlen(token);
			for (size_t i = 0; i < len; i++) {
				cbuild_da_foreach(__cbuild_flag_context.flags, spec) {
					if (spec->spec.short_option == token[i]) {
						spec->found = true;
						if ((i + 1) == len) { // We can parse arguments
							__cbuild_flag_parse_args(spec, argv, &argc);
						} else { // No arguments are possible
							if (spec->spec.optional_arg || spec->spec.num_arguments == 0) {
								// Either we must omit arguments or we can do omit them
							} else {
								cbuild_log_error(
									"Too few arguments for flag -%c, none provided but %d expected.",
									token[i], spec->spec.num_arguments);
								__cbuild_flag_context.help(__cbuild_flag_context.app_name);
								exit(1);
							}
						}
						continue;
					}
				}
				cbuild_log_error("Invalid flag specified: -%c", token[i]);
				__cbuild_flag_context.help(__cbuild_flag_context.app_name);
				exit(1);
			}
		} else { // Positional argument
		}
		cbuild_da_append(&__cbuild_flag_context.pargs, token);
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
// 'offset' is just length of tab in characters.
// 'len' is length of column with flag values, 30 max (if flag is longer than it should just wrap first line a little differently, no need to penalize all flags.
CBUILDDEF char* __cbuild_flag_fmt_flag(struct __cbuild_flag_t* flag, int offset, int len) {
	cbuild_sb_t sb = {0};
	// cbuild_sb_append(&sb, '\t'); // NOTE: This tab should be handled by caller.
	cbuild_sb_appendf(&sb, "--%s", flag->option);
	if (flag->spec.num_arguments) {
		cbuild_sb_append(&sb, ' ');
		if (flag->spec.optional_arg) cbuild_sb_append(&sb, '[');
		else cbuild_sb_append(&sb, '<');
		if (flag->spec.argument_desc) cbuild_sb_append_cstr(&sb, flag->spec.argument_desc);
		else cbuild_sb_append_cstr(&sb, "ARGUMENT");
		if (flag->spec.terminator) {
			cbuild_sb_appendf(&sb, "|%s", flag->spec.terminator);
		}
		if (flag->spec.optional_arg) cbuild_sb_append(&sb, ']');
		else cbuild_sb_append(&sb, '>');
	}
	for (int i = (int)sb.size; i < len; i++) cbuild_sb_append(&sb, ' ');
	cbuild_sb_append(&sb, ' ');
	size_t col1w = sb.size;
	ssize_t desc_len = (ssize_t)strlen(flag->spec.desc);
	const char* desc = flag->spec.desc;
	const size_t line_len = __cbuild_flag_term_width() - col1w - (size_t)offset;
	while (desc_len > 0) {
		if (desc != flag->spec.desc) { // This is means this is not a first iteration
			cbuild_sb_append(&sb, '\t');
			for (size_t i = 0; i < col1w; i++) cbuild_sb_append(&sb, ' ');
		}
		cbuild_sb_append_arr(&sb, desc, CBUILD_MIN(line_len, (size_t)desc_len));
		cbuild_sb_append(&sb, '\n');
		desc += line_len;
		desc_len -= (ssize_t)line_len;
	}
	return sb.data;
}
CBUILDDEF void cbuild_flag_print_help(void) {
	// Measure size of flag name column
	int name_len = 0;
	cbuild_da_foreach(__cbuild_flag_context.flags, flag) {
		int flag_len = 3; // '--' and space after it.
		if (flag->spec.short_option != '\0') flag_len += 4;
		flag_len += (int)strlen(flag->option);
		name_len = CBUILD_MAX(name_len, flag_len);
		if (flag->spec.num_arguments != 0) {
			flag_len += 3; // ' <...>' or ' [...]'
			if (flag->spec.argument_desc) flag_len += (int)strlen(flag->spec.argument_desc);
			else flag_len += 8;
			if (flag->spec.terminator != NULL) {
				flag_len += 1; // 'Terminator is shown as 'desc|terminator'
				flag_len += (int)strlen(flag->spec.terminator);
			}
		}
	}
	name_len = CBUILD_MIN(name_len, 30); // 30 is just arbitrary number.
	// Print ungrouped arguments and collect list of groups
	cbuild_arglist_t groups = {0};
	__CBUILD_PRINT("Flags:\n");
	cbuild_da_foreach(__cbuild_flag_context.flags, flag) {
		if (flag->spec.group == NULL) {
			int x1, y1, x2, y2;
			__cbuild_flag_term_get_cursor(&x1, &y1);
			__CBUILD_PRINT("\t");
			__cbuild_flag_term_get_cursor(&x2, &y2);
			char* desc = __cbuild_flag_fmt_flag(flag, x2 - x1, name_len);
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
			if (strcmp(flag->spec.group, *group) == 0) {
				int x1, y1, x2, y2;
				__cbuild_flag_term_get_cursor(&x1, &y1);
				__CBUILD_PRINT("\t");
				__cbuild_flag_term_get_cursor(&x2, &y2);
				char* desc = __cbuild_flag_fmt_flag(flag, x2 - x1, name_len);
				__CBUILD_PRINT(desc);
				__CBUILD_FREE(desc);
			}
		}
	}
	cbuild_da_clear(&groups);
}
CBUILDDEF cbuild_arglist_t* cbuild_flag_get_pargs(void) {
	return &__cbuild_flag_context.pargs;
}
CBUILDDEF cbuild_arglist_t* cbuild_flag_get_flag(const char* opt) {
	cbuild_da_foreach(__cbuild_flag_context.flags, flag) {
		if (strcmp(flag->option, opt) == 0) {
			if (!flag->found) return NULL;
			return &flag->args;
		}
	}
	return NULL;
}
CBUILDDEF const char* cbuild_flag_app_name(void) {
	return __cbuild_flag_context.app_name;
}
