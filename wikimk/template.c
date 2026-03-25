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
		"[[Look in source line %zu]{.lnum-ref}]("CODE_EDIT_URL")\n",
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
void template_parsed_arg_in_list(cbuild_sb_t* dst, argument_t arg) {
	cbuild_sv_t bare_name = arg.type;
	cbuild_sv_chop_prefix(&bare_name, cbuild_sv_from_lit("const "));
	while (bare_name.data[bare_name.size - 1] == '*') bare_name.size--;
	if (cbuild_sv_prefix(bare_name, cbuild_sv_from_lit("cbuild_"))) {
		cbuild_sb_appendf(dst,
			"[[`"CBuildSVFmt"`]{.c .arglist-type}]("INDEX_URL") ["CBuildSVFmt"]{.arglist-name}",
			CBuildSVArg(arg.type), CBuildSVArg(bare_name), CBuildSVArg(arg.name));
	} else {
		cbuild_sb_appendf(dst,
			"[`"CBuildSVFmt"`]{.c .arglist-type} ["CBuildSVFmt"]{.arglist-name}",
			CBuildSVArg(arg.type), CBuildSVArg(arg.name));
	}
}
void template_parsed_ret(cbuild_sb_t* dst, cbuild_sv_t text) {
	cbuild_sb_appendf(dst, 
		"[Returns]{.return-starter} [`"CBuildSVFmt"`]{.return-type}",
		CBuildSVArg(text));
}

