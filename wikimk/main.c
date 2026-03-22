// Wiki and code doc generator.
// License: `GPL-3.0-or-later`.
// ----------------------------------------
// Config
// ----------------------------------------
// Paths
#define CODE_DOC_SRC "src"
#define CODE_DOC_OUT "wiki/doc"
#define WIKI_SRC "wiki/src"
#define WIKI_OUT "wiki/out"
// Template files
// TODO: Add some sort of templating engine
// Or maybe just provides templates here as functions that just formats
// something like parsing is implemented
// External URLs
#define CODE_EDIT_BASE "https://foo.net/edit?file=%s#L%zu"
#define CODE_EDIT_ARGS(file, line) file, line
// ----------------------------------------
// Header part of code
// ----------------------------------------
// Includes
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#define CBUILD_IMPLEMENTATION
#include "../cbuild.split.h"
// "Header"
typedef struct {
	const char* filename;
	const char* symbol_name;
} symbol_t;
typedef struct {
	symbol_t* data;
	size_t size;
	size_t capacity;
	cbuild_arena_t allocator;
} symbols_t;
typedef struct {
	cbuild_sv_t* data;
	size_t size;
	size_t capacity;
} lines_t;
void push_symbol(symbols_t* symbol_table, cbuild_sv_t name, char* filename);
bool docgen_dir_walk(cbuild_dir_walk_func_args_t args);
bool process_code_file(const char* src, const char* output, symbols_t* symbol_table);
bool create_html_symbol_index(symbols_t* symbol_table, const char* out);
typedef enum {
	DECL_NONE = 0,
	DECL_FUNCTION,
	DECL_VARIABLE,
	DECL_DEFINE,
	DECL_STRUCT,
	DECL_ENUM,
	DECL_TYPEDEF,
	DECL_TYPEDEF_STRUCT,
	DECL_TYPEDEF_ENUM,
	DECL_TYPEDEF_FUNC,
} declaration_type_t; 
typedef struct {
	char* name;
	char* type;
} argument_t;
typedef struct {
	argument_t* data;
	size_t size;
} arguments_t;
bool c_token_delims(const cbuild_sv_t* sv, size_t idx, void* args) {
	if (args != NULL) *(char*)args = sv->data[idx];
	if (isspace(sv->data[idx])) return true;
	if (sv->data[idx] == '(') return true;
	if (sv->data[idx] == ')') return true;
	if (sv->data[idx] == ',') return true;
	if (sv->data[idx] == ';') return true;
	return false;
}
cbuild_sv_t sv_chop_token_with_separator(cbuild_sv_t* sv, char* separator) {
	cbuild_sv_trim_left(sv);
	cbuild_sv_t ret = cbuild_sv_chop_by_func(sv, c_token_delims, separator);
	cbuild_sv_trim_left(sv);
	return ret;
}
cbuild_sv_t sv_chop_token(cbuild_sv_t* sv) {
	return sv_chop_token_with_separator(sv, NULL);
}
// TODO: Move this into CBuild, but this maybe need some group of function
// (eg. multiple functions of such class), so it will be here until I have
// time to thing about thia addition to an API
bool cbuild_sv_chop_prefix(cbuild_sv_t* sv, cbuild_sv_t prefix) {
	if (!cbuild_sv_prefix(*sv, prefix)) return false;
	cbuild_sv_chop(sv, prefix.size);
	return true;
}
size_t cbuild_sv_count(cbuild_sv_t sv, char c) {
	size_t ret = 0;
	for (size_t i = 0; i < sv.size; i++) if (sv.data[i] == c) ret++;
	return ret;
}
// ----------------------------------------
// Custom parse functions
// ----------------------------------------
// All of them should `allocator` return values.
// `decl` is used to return full declaration. It will be literally inserted
// into code-block.
// NOTE: These functions are project-specific
declaration_type_t extract_decl_type(lines_t lines, size_t line);
bool parse_func_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** ret, char** name, arguments_t* args, char** decl);
bool parse_var_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** type, char** name, char** decl);
bool parse_define_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** name, char** decl);
bool parse_struct_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** name, arguments_t* fields, char** decl) {
	CBUILD_UNREACHABLE("unimplemented");
}
bool parse_enum_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** name, cbuild_arglist_t* values, char** decl) {
	CBUILD_UNREACHABLE("unimplemented");
}
bool parse_typedef_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** orig, char** new, char** decl) {
	CBUILD_UNREACHABLE("unimplemented");
}
bool parse_typedef_struct_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** typename, char** name, arguments_t* fields, char** decl) {
	CBUILD_UNREACHABLE("unimplemented");
}
bool parse_typedef_enum_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** typename, char** name, cbuild_arglist_t* values, char** decl) {
	CBUILD_UNREACHABLE("unimplemented");
}
bool parse_typedef_func_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** ret, char** name, arguments_t* args, char** decl);
#include "codeparse.c"
// ----------------------------------------
// Source part of code
// ----------------------------------------
// Main
int main(int argc, char** argv) {
	cbuild_log_info("Processing doc comments.");
	if (!cbuild_dir_check(CODE_DOC_SRC)) {
		cbuild_log_error("Code SRC is invalid. Could not process doc comments.");
		return 1;
	}
	if (!cbuild_dir_check(CODE_DOC_OUT)) {
		cbuild_dir_create(CODE_DOC_OUT);
	}
	if (!cbuild_dir_walk(CODE_DOC_SRC, docgen_dir_walk)) return 1;
	return 0;
}
void push_symbol(symbols_t* symbol_table, cbuild_sv_t name, char* filename) {
	char* str = cbuild_arena_malloc(&symbol_table->allocator, name.size + 1);
	memcpy(str, name.data, name.size);
	str[name.size] = '\0';
	cbuild_da_append(symbol_table, ((symbol_t){
				.symbol_name = str,
				.filename = cbuild_arena_strdup(&symbol_table->allocator, filename),
			}));
}
// Docgen
// NOTE: This uses rather crude check for extension.
bool docgen_dir_walk(cbuild_dir_walk_func_args_t args) {
	if (args.type != CBUILD_FTYPE_REGULAR) return true;
	cbuild_sv_t src = cbuild_sv_from_cstr(args.path);
	if (!cbuild_sv_prefix(src, cbuild_sv_from_lit(CODE_DOC_SRC))) return false;
	if (!cbuild_sv_suffix(src, cbuild_sv_from_lit(".h"))) return true;
	cbuild_sv_chop(&src, sizeof(CODE_DOC_SRC));
	cbuild_sb_t dst = {0};
	cbuild_sb_append_cstr(&dst, CODE_DOC_OUT);
	cbuild_sb_append(&dst, '/');
	cbuild_sb_append_sv(&dst, src);
	dst.size -= 1; // Strip extension
	cbuild_sb_append_cstr(&dst, "md");
	cbuild_sb_append_null(&dst);
	cbuild_log_trace("Converting '%s' into '%s'.", args.path, dst.data);
	if(!process_code_file(args.path, dst.data, NULL)) return false;
	return false;
}
typedef struct {
	lines_t lines;
	size_t first_line;
	size_t last_line; // +1 of last comment line;
	enum {
		COMMENT_NONE = 0,
		COMMENT_FILE,
		COMMENT_DOC,
		COMMENT_ANCHOR,
	} type;
} comment_t;
typedef struct {
	comment_t* data;
	size_t size;
	size_t capacity;
} comments_t;
bool md_format_doc(cbuild_sb_t* sb, comment_t* comment, arguments_t args, char* ret) {
	// Debug
	cbuild_sb_appendf(sb, "Return type: `%s`\n", ret);
	cbuild_sb_appendf(sb, "Arguments:\n");
	cbuild_da_foreach(args, arg) {
		cbuild_sb_appendf(sb, "  - *%s*: `%s`\n", arg->name, arg->type);
	}
	// bool codeblock = false;
	// cbuild_da_foreach(comment->lines, line) {
	// 	if (cbuild_sv_contains_sv(*line, cbuild_sv_from_lit("```"))) {
	// 		codeblock = !codeblock;
	// 	}
	// 	bool duplicate_hash = true;
	// 	for (size_t i = 0; i < line->size; i++) {
	// 		char c = line->data[i];
	// 		if (!codeblock) {
	// 			if (!isspace(c) && duplicate_hash) {
	// 				if (c == '#') cbuild_sb_append_cstr(sb, "##"); 
	// 				duplicate_hash = false;
	// 			}
	// 		}
	// 		cbuild_sb_append(sb, '\n');
	// 	}
	// }
	return true;
}
bool process_code_file(const char* src_, const char* output_, symbols_t* symbol_table) {
	cbuild_sb_t src_sb = {0};
	if (!cbuild_file_read(src_, &src_sb)) return false;
	cbuild_sv_t src = cbuild_sv_from_sb(src_sb);
	size_t lnum = 1;
	comment_t curr = {0};
	comments_t comments = {0};
	lines_t lines = {0};
	while (src.size > 0) {
		cbuild_sv_t line = cbuild_sv_chop_by_delim(&src, '\n');
		cbuild_da_append(&lines, line);
		if (cbuild_sv_prefix(line, cbuild_sv_from_lit("///"))) {
			if (curr.type == COMMENT_NONE) {
				curr.first_line = lnum;
				curr.type = COMMENT_DOC;
			}
			cbuild_sv_chop(&line, 4);
			cbuild_da_append(&curr.lines, line);
		} else if (cbuild_sv_prefix(line, cbuild_sv_from_lit("//!"))) {
			if (curr.type == COMMENT_NONE) {
				curr.first_line = lnum;
				curr.type = COMMENT_FILE;
			}
			cbuild_sv_chop(&line, 4);
			cbuild_da_append(&curr.lines, line);
		} else if (cbuild_sv_prefix(line, cbuild_sv_from_lit("//@"))) {
			if (curr.type == COMMENT_NONE) {
				curr.first_line = lnum;
				curr.type = COMMENT_ANCHOR;
			}
			cbuild_sv_chop(&line, 4);
			cbuild_da_append(&curr.lines, line);
		} else {
			if (curr.type != COMMENT_NONE) {
				curr.last_line = lnum-1;
				// printf("Appended comment:\n");
				// printf("\t%s, %zu:%zu\n",
				// 	curr.type == COMMENT_DOC ? "doc" : curr.type == COMMENT_FILE ? "file" : "anchor",
				// 	curr.first_line, curr.last_line);
				// cbuild_da_foreach(curr.lines, l) {
				// 	printf("\t\t"CBuildSVFmt"\n", CBuildSVArg(*l));
				// }
				cbuild_da_append(&comments, curr);
				memset(&curr, 0, sizeof(curr));
			}
		}
		lnum++;
	}
	cbuild_sb_t dst = {0};
	cbuild_arena_t arena = {0};
	cbuild_arena_base_malloc(&arena, CBUILD_TEMP_ARENA_SIZE);
	cbuild_da_foreach(comments, comment) {
		switch (comment->type) {
		case COMMENT_FILE: {	
			bool codeblock = false;
			bool duplicate_hash = true;
			cbuild_sb_t tag = {0};
			cbuild_da_foreach(comment->lines, cline) {
				if (cbuild_sv_contains_sv(*cline, cbuild_sv_from_lit("```"))) {
					codeblock = !codeblock;
				}
				for (size_t i = 0; i < cline->size; i++) {
					char c = cline->data[i];
					if (!codeblock) {
						if (!isspace(c) && duplicate_hash) {
							if (c == '#') cbuild_sb_append(&dst, '#'); 
							duplicate_hash = false;
						}
					}
					if (c == '[' && (cline->size - i) > 6 &&
						memcmp(&cline->data[i], "[line:", 6) == 0) {
						i += 6;
						while (i < cline->size && cline->data[i] != ']') {
							cbuild_sb_append(&tag, cline->data[i]);
							i++;
						}
						cbuild_da_foreach(comments, cm) {
							if (cm->type == COMMENT_ANCHOR && 
								cbuild_sv_cmp(cbuild_sv_from_sb(tag), cm->lines.data[0]) == 0) {
								cbuild_sb_appendf(&dst, 
									"[[Look in source line %zu]{.lnum-ref}]("
									CODE_EDIT_BASE
									")\n",
									cm->first_line, CODE_EDIT_ARGS(src_, cm->first_line));
							}
						}
						tag.size = 0;
						continue;
					}
					cbuild_sb_append(&dst, c);
				} 
				cbuild_sb_append(&dst, '\n');
				duplicate_hash = true;
			}
			cbuild_sb_append(&dst, '\n');
			cbuild_sb_clear(&tag);
		} break;
		case COMMENT_DOC: {
			cbuild_arena_free(&arena);
			declaration_type_t decltype = extract_decl_type(lines, comment->last_line);
			char *ret = NULL, *name = NULL, *decl = NULL;
			arguments_t args = {0};
			switch (decltype) {
			case DECL_NONE:
				CBUILD_UNREACHABLE("unimplemented");
				break;
			case DECL_FUNCTION: {
				if (!parse_func_decl(lines, comment->last_line,
						&arena, &ret, &name, &args, &decl)) {
					cbuild_log_error("Could not parse function declaration at line %zu.",
						comment->last_line);
					return false;
				}
				cbuild_sb_append_cstr(&dst, "::: {.decl .func-decl}\n");
			} break;
			case DECL_VARIABLE: {
				if (!parse_var_decl(lines, comment->last_line,
						&arena, &ret, &name, &decl)) {
					cbuild_log_error("Could not parse variable declaration at line %zu.",
						comment->last_line);
					return false;
				}
				cbuild_sb_append_cstr(&dst, "::: {.decl .var-decl}\n");
			} break;
			case DECL_DEFINE: {
				if (!parse_define_decl(lines, comment->last_line,
						&arena, &name, &decl)) {
					cbuild_log_error("Could not parse macro declaration at line %zu.",
						comment->last_line);
					return false;
				}
				cbuild_sb_append_cstr(&dst, "::: {.decl .define-decl}\n");
			} break;
			case DECL_STRUCT:
				CBUILD_UNREACHABLE("unimplemented");
				break;
			case DECL_ENUM:
				CBUILD_UNREACHABLE("unimplemented");
				break;
			case DECL_TYPEDEF:
				CBUILD_UNREACHABLE("unimplemented");
				break;
			case DECL_TYPEDEF_STRUCT:
				CBUILD_UNREACHABLE("unimplemented");
				break;
			case DECL_TYPEDEF_ENUM:
				CBUILD_UNREACHABLE("unimplemented");
				break;
			case DECL_TYPEDEF_FUNC: {
				if (!parse_typedef_func_decl(lines, comment->last_line,
						&arena, &ret, &name, &args, &decl)) {
					cbuild_log_error("Could not parse function typedef declaration at line %zu.",
						comment->last_line);
					return false;
				}
				cbuild_sb_append_cstr(&dst, "::: {.decl .typedef-func-decl}\n");
			} break;
			}
			cbuild_sb_appendf(&dst, "## %s {#%s}\n", name, name);
			cbuild_sb_appendf(&dst, "``` c\n%s\n```\n", decl);
			if (!md_format_doc(&dst, comment, args, ret)) return false;
			cbuild_sb_append_cstr(&dst, ":::\n\n");
		} break;
		case COMMENT_ANCHOR:
			// This is just skipped.
			break;
		case COMMENT_NONE:
		default:
			CBUILD_UNREACHABLE("Invalid code.");
			break;
		}
	}
	// printf("----------------------------------------\n");
	// printf(CBuildSBFmt, CBuildSBArg(dst));
	// printf("----------------------------------------\n");
	cbuild_arena_base_free(&arena);
	if (!cbuild_file_write(output_, &dst)) return false;
	return true;
}
bool create_html_symbol_index(symbols_t* symbol_table, const char* out);
