// Template for wikimk
// License: `GPL-3.0-or-later`.
// Includes
#include "wikimk.h"
#include "../cbuild.split.h"
// Helper
#define CODE_EDIT_URL "https://foo.net/edit?file=%s#L%zu"
#define INDEX_URL "symbols.html#ID_"CBuildSVFmt
// Code edit URL
void template_code_edit(cbuild_sb_t* dst, const char* file, size_t line) {
	cbuild_sb_appendf(dst, CODE_EDIT_URL, file, line);
}
void template_look_in_source(cbuild_sb_t* dst, const char* file, size_t line) {
	cbuild_sb_appendf(dst,
		"[[Look in source line %zu]("CODE_EDIT_URL")]{.lnum-ref}\n",
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
void template_decl_name(cbuild_sb_t* dst, cbuild_sv_t name) {
	cbuild_sb_appendf(dst, "## "CBuildSVFmt" {#ID_"CBuildSVFmt"}\n\n",
		CBuildSVArg(name), CBuildSVArg(name));
}
void template_decl_code(cbuild_sb_t* dst, cbuild_sv_t decl_raw) {
	cbuild_sb_appendf(dst, "``` c\n"CBuildSVFmt"\n```\n\n", CBuildSVArg(decl_raw));
}
// Formatting of parsed data
void template_parsed_arg_or_field_in_list(cbuild_sb_t* dst, argument_t arg, const char* what) {
	cbuild_sv_t bare_name = arg.type;
	cbuild_sv_chop_prefix(&bare_name, cbuild_sv_from_lit("const "));
	// This project uses '...<struct name>' to denote structs used to simulate
	// named arguemnts (like in cbuild_cmd_run macro).
	cbuild_sv_chop_prefix(&bare_name, cbuild_sv_from_lit("..."));
	while (bare_name.data[bare_name.size - 1] == '*') bare_name.size--;
	if (cbuild_sv_prefix(bare_name, cbuild_sv_from_lit("cbuild_"))) {
		cbuild_sb_appendf(dst,
			"[[`"CBuildSVFmt"`{.c}]("INDEX_URL")]{.%slist-type} ["CBuildSVFmt"]{.%slist-name}",
			CBuildSVArg(arg.type), CBuildSVArg(bare_name), what, CBuildSVArg(arg.name), what);
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
// TODO: Link on type
void template_parsed_arg_or_field_standalone(cbuild_sb_t* dst, argument_t arg, const char* what) {
	cbuild_sv_t bare_name = arg.type;
	cbuild_sv_chop_prefix(&bare_name, cbuild_sv_from_lit("const "));
	// This project uses '...<struct name>' to denote structs used to simulate
	// named arguemnts (like in cbuild_cmd_run macro).
	cbuild_sv_chop_prefix(&bare_name, cbuild_sv_from_lit("..."));
	while (bare_name.data[bare_name.size - 1] == '*') bare_name.size--;
	if (cbuild_sv_prefix(bare_name, cbuild_sv_from_lit("cbuild_"))) {
		cbuild_sb_appendf(dst,
			"[`"CBuildSVFmt"`[[`"CBuildSVFmt"`{.c}]("INDEX_URL")]{.%sref-type}]{.%sref-name}",
			CBuildSVArg(arg.name), CBuildSVArg(arg.type), CBuildSVArg(bare_name), what, what);
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
	cbuild_sb_appendf(dst, 
		"[Returns]{.return-starter} [`"CBuildSVFmt"`]{.return-type}",
		CBuildSVArg(text));
}
// Symbol table formatting
void template_symtab_entry(cbuild_sb_t* dst, const char* symbol, const char* file) {
	const char* fname = cbuild_path_name(file);
	const char* md_fname = cbuild_temp_sprintf("%.*s.md", 
		(int)(strlen(fname) - 2), fname);
	cbuild_sb_appendf(dst, 
		"## [[`%s`](%s#%s)]{.symbols-list-elem} {#ID_%s}\n",
		symbol, md_fname, symbol, symbol);
}
