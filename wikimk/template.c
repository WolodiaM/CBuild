// Template for wikimk
// License: `GPL-3.0-or-later`.
// Includes
#include "wikimk.h"
#include "../cbuild.split.h"
// Helper
#define CODE_EDIT_URL "https://github.com/WolodiaM/CBuild/blob/master/%s#L%zu"
#define INDEX_URL "symbols.html#ID_"CBuildSVFmt
// Helpers
cbuild_sv_t TEMPLATE_CROSSREF_EXCLUDED_SYMBOLS[] = {
	cbuild_sv_from_lit("cbuild_da_t"),
	cbuild_sv_from_lit("cbuild_stack_t"),
};
bool template_gen_crossref_name(cbuild_sv_t name, cbuild_sv_t* out) {
	cbuild_sv_chop_prefix(&name, cbuild_sv_from_lit("const "));
	// This project uses '...<struct name>' to denote structs used to simulate
	// named arguemnts (like in cbuild_cmd_run macro).
	cbuild_sv_chop_prefix(&name, cbuild_sv_from_lit("..."));
	while (name.data[name.size - 1] == '*') name.size--;
	if (!cbuild_sv_prefix(name, cbuild_sv_from_lit("cbuild_"))) return false;
	for (size_t i = 0; i < cbuild_arr_len(TEMPLATE_CROSSREF_EXCLUDED_SYMBOLS); i++) {
		if (cbuild_sv_cmp(name, TEMPLATE_CROSSREF_EXCLUDED_SYMBOLS[i]) == 0) return false;
	}
	*out = name;
	return true;
}
// Code edit URL
void template_code_edit(cbuild_sb_t* dst, const char* file, size_t line) {
	cbuild_sb_appendf(dst, CODE_EDIT_URL, file, line);
}
void template_look_in_source(cbuild_sb_t* dst, const char* file, size_t line) {
	cbuild_sb_appendf(dst,
		"[[Look in source line %zu]("CODE_EDIT_URL")]{.lnum-ref}",
		line, file, line);
}
// Div for each declaration
void template_func_decl_start(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, "::: {.decl .func-decl}\n");
}
void template_var_decl_start(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, "::: {.decl .var-decl}\n");
}
void template_define_decl_start(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, "::: {.decl .define-decl}\n");
}
void template_struct_decl_start(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, "::: {.decl .struct-decl}\n");
}
void template_enum_decl_start(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, "::: {.decl .enum-decl}\n");
}
void template_typedef_decl_start(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, "::: {.decl .typedef-decl}\n");
}
void template_typedef_struct_decl_start(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, "::: {.decl .typedef-struct-decl}\n");
}
void template_typedef_enum_decl_start(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, "::: {.decl .typedef-enum-decl}\n");
}
void template_typedef_func_decl_start(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, "::: {.decl .typedef-func-decl}\n");
}
void template_decl_end(cbuild_sb_t* dst) {
	cbuild_sb_append_cstr(dst, ":::\n\n");
}
void template_decl_name(cbuild_sb_t* dst, cbuild_sv_t name, const char* src, size_t line) {
	cbuild_sb_append_cstr(dst, "## [");
	for (size_t i = 0; i < name.size; i++) {
		char c = name.data[i];
		if (c == '_') cbuild_sb_append(dst, '\\');
		cbuild_sb_append(dst, c);
	}
	cbuild_sb_append_cstr(dst, "](");
	template_code_edit(dst, src, line);
	cbuild_sb_appendf(dst, ") {#ID_"CBuildSVFmt"}\n\n", CBuildSVArg(name));
}
void template_decl_code(cbuild_sb_t* dst, cbuild_sv_t decl_raw) {
	cbuild_sb_appendf(dst, "``` c\n"CBuildSVFmt"\n```\n\n", CBuildSVArg(decl_raw));
}
// Formatting of parsed data
void template_parsed_arg_or_field_in_list(cbuild_sb_t* dst, argument_t arg, const char* what) {
	cbuild_sv_t ref = {0};
	if (template_gen_crossref_name(arg.type, &ref)) {
		cbuild_sb_appendf(dst,
			"[[`"CBuildSVFmt"`{.c}]("INDEX_URL")]{.%slist-type} ["CBuildSVFmt"]{.%slist-name}",
			CBuildSVArg(arg.type), CBuildSVArg(ref), what, CBuildSVArg(arg.name), what);
	} else {
		cbuild_sb_appendf(dst,
			"[`"CBuildSVFmt"`{.c}]{.%slist-type} ["CBuildSVFmt"]{.%slist-name}",
			CBuildSVArg(arg.type), what, CBuildSVArg(arg.name), what);
	}
}
void template_parsed_arg_in_list(cbuild_sb_t* dst, argument_t arg) {
	template_parsed_arg_or_field_in_list(dst, arg, "arg");
}
void template_parsed_field_in_list(cbuild_sb_t* dst, argument_t arg) {
	template_parsed_arg_or_field_in_list(dst, arg, "field");
}
void template_parsed_enum_value_in_list(cbuild_sb_t* dst, argument_t arg) {
	cbuild_sb_appendf(dst, "["CBuildSVFmt"]{.enumval-list}", CBuildSVArg(arg.name));
}
void template_parsed_arg_or_field_standalone(cbuild_sb_t* dst, argument_t arg, const char* what) {
	cbuild_sv_t ref = {0};
	if (template_gen_crossref_name(arg.type, &ref)) {
		cbuild_sb_appendf(dst,
			"[`"CBuildSVFmt"`[[`"CBuildSVFmt"`{.c}]("INDEX_URL")]{.%sref-type}]{.%sref-name}",
			CBuildSVArg(arg.name), CBuildSVArg(arg.type), CBuildSVArg(ref), what, what);
	} else {
		cbuild_sb_appendf(dst,
			"[`"CBuildSVFmt"`[`"CBuildSVFmt"`{.c}]{.%sref-type}]{.%sref-name}",
			CBuildSVArg(arg.name), CBuildSVArg(arg.type), what, what);
	}
}
void template_parsed_arg_standalone(cbuild_sb_t* dst, argument_t arg) {
	template_parsed_arg_or_field_standalone(dst, arg, "arg");
}
void template_parsed_field_standalone(cbuild_sb_t* dst, argument_t arg) {
	template_parsed_arg_or_field_standalone(dst, arg, "field");
}
void template_parsed_enum_value_standalone(cbuild_sb_t* dst, argument_t arg) {
	cbuild_sb_appendf(dst, "[`"CBuildSVFmt"`]{.enumval-ref}", CBuildSVArg(arg.name));
}
void template_parsed_ret(cbuild_sb_t* dst, cbuild_sv_t text) {
	cbuild_sb_appendf(dst, "[`"CBuildSVFmt"`]{.return-type}", CBuildSVArg(text));
}
// Symbol table formatting
void template_symtab_entry(cbuild_sb_t* dst, const char* symbol, const char* file) {
	const char* fname = cbuild_path_name(file);
	const char* html_fname = cbuild_temp_sprintf("%.*s.html", 
		(int)(strlen(fname) - 2), fname);
	cbuild_sb_appendf(dst, 
		"### [[`%s`](%s#ID_%s)]{.symbols-list-elem} {#ID_%s}\n",
		symbol, html_fname, symbol, symbol);
}
