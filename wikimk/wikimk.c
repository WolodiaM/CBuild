// Wiki and code doc generator.
// License: `GPL-3.0-or-later`.
// ----------------------------------------
// Shared types and declarations
// ----------------------------------------
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "wikimk.h"
// ----------------------------------------
// Includes
// ----------------------------------------
#define CBUILD_IMPLEMENTATION
#include "../cbuild.split.h"
// ----------------------------------------
// Config
// ----------------------------------------
// Paths
#define CODE_DOC_SRC "foo"
#define CODE_DOC_OUT "wiki/doc"
#define WIKI_SRC "wiki/src"
#define WIKI_OUT "wiki/out"
// Template
struct {
	cbuild_dlib_t lib;
	// Code edit URL
	void (*code_edit)(cbuild_sb_t* dst, const char* file, size_t line);
	void (*look_in_source)(cbuild_sb_t* dst, const char* file, size_t line);
	// Div for each declaration
	void (*func_decl_start)(cbuild_sb_t* dst);
	void (*var_decl_start)(cbuild_sb_t* dst);
	void (*define_decl_start)(cbuild_sb_t* dst);
	void (*struct_decl_start)(cbuild_sb_t* dst);
	void (*enum_decl_start)(cbuild_sb_t* dst);
	void (*typedef_decl_start)(cbuild_sb_t* dst);
	void (*typedef_struct_decl_start)(cbuild_sb_t* dst);
	void (*typedef_enum_decl_start)(cbuild_sb_t* dst);
	void (*typedef_func_decl_start)(cbuild_sb_t* dst);
	void (*decl_end)(cbuild_sb_t* dst);
	void (*decl_name)(cbuild_sb_t* dst, cbuild_sv_t name);
	void (*decl_code)(cbuild_sb_t*dst, cbuild_sv_t decl_raw);
	// Formatting of parsed data
	void (*parsed_arg_in_list)(cbuild_sb_t* dst, argument_t arg);
	void (*parsed_ret)(cbuild_sb_t* dst, cbuild_sv_t text);
} TEMPLATE = {0};
struct {
	cbuild_dlib_t lib;
	declaration_type_t (*decl_type)(lines_t* lines, size_t line);
	bool (*func_decl)(lines_t* lines, size_t line,
		cbuild_sv_t* ret, cbuild_sv_t* name, arguments_t* args, cbuild_sv_t* decl);
	bool (*var_decl)(lines_t* lines, size_t line,
		cbuild_sv_t* type, cbuild_sv_t* name, cbuild_sv_t* decl);
	bool (*define_decl)(lines_t* lines, size_t line,
		cbuild_sv_t* name, cbuild_sv_t* decl);
	bool (*struct_decl)(lines_t* lines, size_t line,
		cbuild_sv_t* name, arguments_t* fields, cbuild_sv_t* decl);
	bool (*enum_decl)(lines_t* lines, size_t line,
		cbuild_sv_t* name, arguments_t* values, cbuild_sv_t* decl);
	bool (*typedef_decl)(lines_t* lines, size_t line,
		cbuild_sv_t* orig, cbuild_sv_t* new, cbuild_sv_t* decl);
	bool (*typedef_struct_decl)(lines_t* lines, size_t line,
		cbuild_sv_t* typename, cbuild_sv_t* name, arguments_t* fields, cbuild_sv_t* decl);
	bool (*typedef_enum_decl)(lines_t* lines, size_t line,
		cbuild_sv_t* typename, cbuild_sv_t* name, arguments_t* values, cbuild_sv_t* decl);
	bool (*typedef_func_decl)(lines_t* lines, size_t line,
		cbuild_sv_t* ret, cbuild_sv_t* name, arguments_t* fields, cbuild_sv_t* decl);
} PARSER = {0};
// ----------------------------------------
// Header part of code
// ----------------------------------------
// "Header"
typedef struct {
	const char* filename;
	const char* symbol_name;
} symbol_t;
typedef struct {
	symbol_t* data;
	size_t size;
	size_t capacity;
	cbuild_arena_t arena;
} symbols_t;
bool docgen_dir_walk(cbuild_dir_walk_func_args_t args);
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
char cbuild_sv_chop_char(cbuild_sv_t* sv) {
	char c = *sv->data;
	sv->data++;
	sv->size--;
	return c;
}
// ----------------------------------------
// Source part of code
// ----------------------------------------
// Loading of "configs"
bool load_template(void) {
	TEMPLATE.lib = cbuild_dlib_open("template.so", false);
	if (TEMPLATE.lib == NULL) {
		cbuild_log_error("Could not open shared library \"template.so\".");
		return false;
	}
	TEMPLATE.code_edit = cbuild_dlib_get_sym(TEMPLATE.lib, "template_code_edit");
	TEMPLATE.look_in_source = cbuild_dlib_get_sym(TEMPLATE.lib, "template_look_in_source");
	TEMPLATE.func_decl_start = cbuild_dlib_get_sym(TEMPLATE.lib, "template_func_decl_start");
	TEMPLATE.var_decl_start = cbuild_dlib_get_sym(TEMPLATE.lib, "template_var_decl_start");
	TEMPLATE.define_decl_start = cbuild_dlib_get_sym(TEMPLATE.lib, "template_define_decl_start");
	TEMPLATE.struct_decl_start = cbuild_dlib_get_sym(TEMPLATE.lib, "template_struct_decl_start");
	TEMPLATE.enum_decl_start = cbuild_dlib_get_sym(TEMPLATE.lib, "template_enum_decl_start");
	TEMPLATE.typedef_decl_start = cbuild_dlib_get_sym(TEMPLATE.lib, "template_typedef_decl_start");
	TEMPLATE.typedef_struct_decl_start = cbuild_dlib_get_sym(TEMPLATE.lib, "template_typedef_struct_decl_start");
	TEMPLATE.typedef_enum_decl_start = cbuild_dlib_get_sym(TEMPLATE.lib, "template_typedef_enum_decl_start");
	TEMPLATE.typedef_func_decl_start = cbuild_dlib_get_sym(TEMPLATE.lib, "template_typedef_func_decl_start");
	TEMPLATE.decl_end = cbuild_dlib_get_sym(TEMPLATE.lib, "template_decl_end");
	TEMPLATE.decl_name = cbuild_dlib_get_sym(TEMPLATE.lib, "template_decl_name");
	TEMPLATE.decl_code = cbuild_dlib_get_sym(TEMPLATE.lib, "template_decl_code");
	TEMPLATE.parsed_arg_in_list = cbuild_dlib_get_sym(TEMPLATE.lib, "template_parsed_arg_in_list");
	TEMPLATE.parsed_ret = cbuild_dlib_get_sym(TEMPLATE.lib, "template_parsed_ret");
	return true;
}
bool load_parser(void) {
	PARSER.lib = cbuild_dlib_open("codeparse.so", false);
	if (PARSER.lib == NULL) {
		cbuild_log_error("Could not open shared library \"codeparse.so\".");
		return false;
	}
	PARSER.decl_type = cbuild_dlib_get_sym(PARSER.lib, "parser_decl_type");
	PARSER.func_decl = cbuild_dlib_get_sym(PARSER.lib, "parser_func_decl");
	PARSER.var_decl = cbuild_dlib_get_sym(PARSER.lib, "parser_var_decl");
	PARSER.define_decl = cbuild_dlib_get_sym(PARSER.lib, "parser_define_decl");
	PARSER.struct_decl = cbuild_dlib_get_sym(PARSER.lib, "parser_struct_decl");
	PARSER.enum_decl = cbuild_dlib_get_sym(PARSER.lib, "parser_enum_decl");
	PARSER.typedef_decl = cbuild_dlib_get_sym(PARSER.lib, "parser_typedef_decl");
	PARSER.typedef_struct_decl = cbuild_dlib_get_sym(PARSER.lib, "parser_typedef_struct_decl");
	PARSER.typedef_enum_decl = cbuild_dlib_get_sym(PARSER.lib, "parser_typedef_enum_decl");
	PARSER.typedef_func_decl = cbuild_dlib_get_sym(PARSER.lib, "parser_typedef_func_decl");
	return true;
}
// Main
int main(int argc, char** argv) {
	if (!load_template()) return 1;
	if (!load_parser()) return 1;
	cbuild_log_info("Processing doc comments.");
	if (!cbuild_dir_check(CODE_DOC_SRC)) {
		cbuild_log_error("Code SRC is invalid. Could not process doc comments.");
		return 1;
	}
	if (!cbuild_dir_check(CODE_DOC_OUT)) {
		cbuild_dir_create(CODE_DOC_OUT);
	}
	if (!cbuild_dir_walk(CODE_DOC_SRC, docgen_dir_walk)) return 1;
	cbuild_dlib_close(PARSER.lib);
	cbuild_dlib_close(TEMPLATE.lib);
	return 0;
}
void push_symbol(symbols_t* symbol_table, cbuild_sv_t name, const char* filename) {
	char* str = cbuild_arena_malloc(&symbol_table->arena, name.size + 1);
	memcpy(str, name.data, name.size);
	str[name.size] = '\0';
	cbuild_da_append(symbol_table, ((symbol_t){
				.symbol_name = str,
				.filename = cbuild_arena_strdup(&symbol_table->arena, filename),
			}));
}
// ----------------------------------------
// Docgen
// ----------------------------------------
// Helpers
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
typedef struct {
	lines_t lines;
	size_t first_line;
	size_t last_line; // +1 of last comment line
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
// Parse and extract comments out of file
// NOTE: This assumes one space between comment starter and comment body
bool docgen_parse_file(const char* src_, void** file, comments_t* comments, lines_t* lines) {
	cbuild_sb_t src_sb = {0};
	if (!cbuild_file_read(src_, &src_sb)) return false;
	cbuild_sv_t src = cbuild_sv_from_sb(src_sb);
	size_t lnum = 1;
	comment_t curr = {0};
	while (src.size > 0) {
		cbuild_sv_t line = cbuild_sv_chop_by_delim(&src, '\n');
		cbuild_da_append(lines, line);
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
				cbuild_da_append(comments, curr);
				memset(&curr, 0, sizeof(curr));
			}
		}
		lnum++;
	}
	*file = src_sb.data;
	return true;
}
bool md_dsl_arg_delim(const cbuild_sv_t* sv, size_t idx, void* args) {
	if (args != NULL) *(char*)args = sv->data[idx];
	if (sv->data[idx] == ':') return true;
	if (sv->data[idx] == ']') return true;
	return false;
}
// Base formatters for comments
bool docgen_comment_to_md(const comment_t* comment, cbuild_sb_t* dst,
	const char* src, const comments_t* comments, const char* header_prefix,
	const cbuild_sv_t* pret_or_ptypename, const cbuild_sv_t* pname,
	const arguments_t* pargs) {
	bool codeblock = false;
	bool duplicate_hash = true;
	cbuild_da_foreach(comment->lines, cline) {
		cbuild_sv_t line = *cline;
		if (cbuild_sv_contains_sv(line, cbuild_sv_from_lit("```"))) {
			codeblock = !codeblock;
		}
		while (line.size > 0) {
			char c = cbuild_sv_chop_char(&line);
			// Check only if not inside of codeblock
			if (!codeblock) {
				if (!isspace(c) && duplicate_hash) {
					if (c == '#') cbuild_sb_append_cstr(dst, header_prefix); 
					duplicate_hash = false;
				}
				// This can start a command
				if (c == '[') {
					if (cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("line:"))) {
						cbuild_sv_t tag = cbuild_sv_chop_by_delim(&line, ']');
						cbuild_da_foreach(*comments, cm) {
							if (cm->type == COMMENT_ANCHOR && 
								cbuild_sv_cmp(tag, cm->lines.data[0]) == 0) {
								TEMPLATE.look_in_source(dst, src, cm->first_line);
							}
						}
					} else if (cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("pl:"))) {
						char sep = '\0';
						cbuild_sv_t arg = cbuild_sv_chop_by_func(&line, md_dsl_arg_delim, &sep);
						if (sep == ':') {
							CBUILD_TODO("unimplemented");
						} else if (sep == ']') {
							bool found = false;
							if (pargs) {
								cbuild_da_foreach(*pargs, a) {
									if (cbuild_sv_cmp(arg, a->name) == 0) {
										found = true;
										TEMPLATE.parsed_arg_in_list(dst, *a);
										break;
									}
								}
							}
							argument_t no_arg = {
								.type = cbuild_sv_from_lit("void"),
								.name = cbuild_sv_from_lit("no-name"),
							};
							if (!found) TEMPLATE.parsed_arg_in_list(dst, no_arg);
						} else {
							CBUILD_UNREACHABLE("Invalid markdown DSL separator.");
						}
					} else if (cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("r:"))) {
						// This is technically out of bound, but we chopped two chars at least,
						// so this access is valid.
						if (*line.data == ']') {
							CBUILD_TODO("unimplemented");
						} else {
							cbuild_sv_t text = cbuild_sv_chop_by_delim(&line, ']');
							TEMPLATE.parsed_ret(dst, text);
						}
					} else {
						goto not_a_command;
					}
					continue;
				}
			}
		not_a_command:
			cbuild_sb_append(dst, c);
		}
		cbuild_sb_append(dst, '\n');
		duplicate_hash = true;
	} 
	cbuild_sb_append(dst, '\n');
	return true;
}
bool docgen_generate_declaration_doc(const comment_t* comment, cbuild_sb_t* dst,
	lines_t* lines, const char* src, const comments_t* comments,
	symbols_t* symbol_table) {
	declaration_type_t decltype = PARSER.decl_type(lines, comment->last_line);
	cbuild_sv_t ret_or_typename = {0}, name = {0}, decl = {0};
	arguments_t args = {0};
	switch (decltype) {
	case DECL_FUNCTION: {
		if (!PARSER.func_decl(lines, comment->last_line,
				&ret_or_typename, &name, &args, &decl)) {
			cbuild_log_error("Could not parse function declaration at line %zu.",
				comment->last_line);
			return false;
		}
		TEMPLATE.func_decl_start(dst);
	} break;
	case DECL_VARIABLE: {
		if (!PARSER.var_decl(lines, comment->last_line,
				&ret_or_typename, &name, &decl)) {
			cbuild_log_error("Could not parse variable declaration at line %zu.",
				comment->last_line);
			return false;
		}
		TEMPLATE.var_decl_start(dst);
	} break;
	case DECL_DEFINE: {
		if (!PARSER.define_decl(lines, comment->last_line,
				&name, &decl)) {
			cbuild_log_error("Could not parse macro declaration at line %zu.",
				comment->last_line);
			return false;
		}
		TEMPLATE.define_decl_start(dst);
	} break;
	case DECL_STRUCT:
		CBUILD_TODO("unimplemented");
		break;
	case DECL_ENUM:
		CBUILD_TODO("unimplemented");
		break;
	case DECL_TYPEDEF:
		CBUILD_TODO("unimplemented");
		break;
	case DECL_TYPEDEF_STRUCT: {
		if (!PARSER.typedef_struct_decl(lines, comment->last_line,
				&ret_or_typename, &name, &args, &decl)) {
			cbuild_log_error("Could not parse struct typedef declaration at line %zu.",
				comment->last_line);
			return false;
		}
		TEMPLATE.typedef_struct_decl_start(dst);
	} break;
	case DECL_TYPEDEF_ENUM:
		CBUILD_TODO("unimplemented");
		break;
	case DECL_TYPEDEF_FUNC: {
		if (!PARSER.typedef_func_decl(lines, comment->last_line,
				&ret_or_typename, &name, &args, &decl)) {
			cbuild_log_error("Could not parse function typedef declaration at line %zu.",
				comment->last_line);
			return false;
		}
		TEMPLATE.typedef_func_decl_start(dst);
	} break;
	case DECL_NONE:
		CBUILD_UNREACHABLE("Invalid declaration type.");
		break;
	}
	// push_symbol(symbol_table, name, src);
	TEMPLATE.decl_name(dst, name);
	TEMPLATE.decl_code(dst, decl);
	if (!docgen_comment_to_md(comment, dst, src, comments, "##", &ret_or_typename, &name, &args)) {
		return false;
	}
	cbuild_sb_appendf(dst, "Ret/Typename: "CBuildSVFmt"\n", CBuildSVArg(ret_or_typename));
	cbuild_sb_appendf(dst, "Name: "CBuildSVFmt"\n", CBuildSVArg(name));
	cbuild_sb_append_cstr(dst, "Arguments:\n");
	cbuild_da_foreach(args, arg) {
		cbuild_sb_appendf(dst, "  - "CBuildSVFmt" of "CBuildSVFmt"\n",
			CBuildSVArg(arg->name), CBuildSVArg(arg->type));
	}
	TEMPLATE.decl_end(dst);
	cbuild_da_foreach(args, arg) {
		free((void*)arg->type.data);
		free((void*)arg->name.data);
	}
	free((void*)decl.data);
	free((void*)ret_or_typename.data);
	free((void*)name.data);
	free((void*)args.data);
	return true;
}
// Dispatchers of docgen
bool docgen_process_file(const char* src, const char* output, symbols_t* symbol_table) {
	comments_t comments = {0};
	lines_t lines = {0};
	cbuild_sb_t dst = {0};
	void* file_content = NULL;
	if (!docgen_parse_file(src, &file_content, &comments, &lines)) return false;
	cbuild_da_foreach(comments, comment) {
		switch (comment->type) {
		case COMMENT_FILE: {
			if (!docgen_comment_to_md(comment, &dst, src, &comments, "#", NULL, NULL, NULL)) {
				return false;
			}
		} break;
		case COMMENT_DOC: {
			if (!docgen_generate_declaration_doc(comment, &dst, &lines, 
					src, &comments, symbol_table)) {
				return false;
			}
		} break;
		case COMMENT_ANCHOR:
			// This comment is using only for marking location and produce no output.
			break;
		case COMMENT_NONE:
			CBUILD_UNREACHABLE("Invalid comment type encountered.");
		}
	}
	if (!cbuild_file_write(output, &dst)) return false;
	cbuild_sb_clear(&dst);
	cbuild_da_foreach(comments, comment) {
		cbuild_da_clear(&comment->lines);
	}
	cbuild_da_clear(&comments);
	cbuild_da_clear(&lines);
	free(file_content);
	return true;
}
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
	if(!docgen_process_file(args.path, dst.data, NULL)) return false;
	cbuild_sb_clear(&dst);
	return true;
}
