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
CBUILDDEF void cbuild_flag_parse(int argc, char** argv);
// 'offset' is just length of tab in characters.
// 'len' is length of column with flag values, 30 max (if flag is longer than it should just wrap first line a little differently, no need to penalize all flags.
CBUILDDEF char* __cbuild_flag_fmt_flag(struct __cbuild_flag_t* flag, int offset, int len) {
	cbuild_sb_t sb = {0};
	// cbuild_sb_append(&sb, '\t'); // NOTE: This tab should be handled by caller.
	int len_adj = 2; // For '--'
	if (flag->spec.short_option != '\0') {
		cbuild_sb_appendf(&sb, "-%c, ", flag->spec.short_option);
		len_adj += 4;
	}
	cbuild_sb_appendf(&sb, "--%s", flag->option);
	if (flag->spec.num_arguments) {
		cbuild_sb_append(&sb, ' ');
		if (flag->spec.optional_arg) cbuild_sb_append(&sb, '[');
		else cbuild_sb_append(&sb, '<');
		cbuild_sb_append_cstr(&sb, flag->spec.argument_desc);
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
		desc_len -= line_len;
	}
	return sb.data;
}
CBUILDDEF void cbuild_flag_print_help(void) {
	// Measure size of flag name column
	int name_len = 0;
	cbuild_da_foreach(__cbuild_flag_context.flags, flag) {
		int flag_len = 3; // '--' and space after it.
		if (flag->spec.short_option != '\0') flag_len += 4;
		flag_len += strlen(flag->option);
		name_len = CBUILD_MAX(name_len, flag_len);
		if (flag->spec.num_arguments != 0) {
			flag_len += 3; // ' <...>' or ' [...]'
			flag_len += strlen(flag->spec.argument_desc);
			if (flag->spec.terminator != NULL) {
				flag_len += 1; // 'Terminator is shown as 'desc|terminator'
				flag_len += strlen(flag->spec.terminator);
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
	cbuild_sb_clear(&groups);
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
