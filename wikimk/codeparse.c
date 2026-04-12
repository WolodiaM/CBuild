// C declaration parser for CBuild.
// This file can parse declarations from CBuild, not C declarations in general.
// License: `GPL-3.0-or-later`.
// Includes
#include "../cbuild.split.h"
#include "wikimk.h"
// TODO: All errors should print current line.
// TODO: Code in nested composite parsing is ugly. Too much duplication.
// NOTE: All cbuild_sv_t returned own their data (and are guaranteed to be constant
// on caller side) and should be allocated with 'malloc'.
// Code
cbuild_sv_t sb_leak_as_sv(cbuild_sb_t* sb) {
	cbuild_sv_t ret = cbuild_sv_from_sb(*sb);
	sb->data = NULL;
	sb->size = 0;
	sb->capacity = 0;
	return ret;
}
cbuild_sv_t sv_dup(cbuild_sv_t sv) {
	char* data = malloc(sv.size);
	cbuild_assert(data != NULL, "Allocation failed.");
	memcpy(data, sv.data, sv.size);
	return cbuild_sv_from_parts(data, sv.size);
}
bool c_token_delims(const cbuild_sv_t* sv, size_t idx, void* args) {
	if (args != NULL) *(char*)args = sv->data[idx];
	if (isspace((unsigned char)sv->data[idx])) return true;
	if (sv->data[idx] == '(') return true; // Function decl uses this
	if (sv->data[idx] == ')') return true; // Function decl uses this
	if (sv->data[idx] == ',') return true; // Argument list decl uses this
	if (sv->data[idx] == ';') return true; // Just a full-stop in C term
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
// This should be used for all reads from lines except for first one
void next_line(lines_t* lines, size_t* line, cbuild_sv_t* cline, cbuild_sb_t* decl) {
	while (true) {
		cbuild_assert(*line <= lines->size,
			"Overflow in lines array: %zu > %zu.\n", *line, lines->size);
		*cline = lines->data[(*line)++];
		cbuild_sv_t cl = *cline;
		cbuild_sv_trim_left(&cl);
		if (cl.size == 0) continue;
		cbuild_sv_trim_right(cline);
		cbuild_sb_append(decl, '\n');
		cbuild_sb_append_sv(decl, *cline);
		if (cbuild_sv_prefix(cl, cbuild_sv_from_lit("//"))) continue;
		if (cbuild_sv_prefix(cl, cbuild_sv_from_lit("/*"))) continue;
		if (*cl.data == '#') continue;
		break;
	}
	cbuild_sv_trim_left(cline);
}
declaration_type_t parser_decl_type(lines_t* lines, size_t line) {
	cbuild_sv_t cline = lines->data[line];
	cbuild_sv_trim_left(&cline);
	if (cbuild_sv_prefix(cline, cbuild_sv_from_lit("#define"))) {
		return DECL_DEFINE;
	} else if (cbuild_sv_prefix(cline, cbuild_sv_from_lit("struct"))) {
		return DECL_STRUCT;
	} else if (cbuild_sv_prefix(cline, cbuild_sv_from_lit("enum"))) {
		return DECL_ENUM;
	} else if (cbuild_sv_prefix(cline, cbuild_sv_from_lit("typedef"))) {
		cbuild_sv_chop_by_delim(&cline, ' ');
		cbuild_sv_trim_left(&cline);
		if (cbuild_sv_prefix(cline, cbuild_sv_from_lit("struct"))) {
			return DECL_TYPEDEF_STRUCT;
		} else if (cbuild_sv_prefix(cline, cbuild_sv_from_lit("enum"))) {
			return DECL_TYPEDEF_ENUM;
		} else if (cbuild_sv_contains_sv(cline, cbuild_sv_from_lit("(*"))) {
			return DECL_TYPEDEF_FUNC;
		} else {
			return DECL_TYPEDEF;
		}
	} else if (cbuild_sv_prefix(cline, cbuild_sv_from_lit("extern"))) {
		// All global variables in CBuild are declared `extern`
		return DECL_VARIABLE;
	} else {
		return DECL_FUNCTION;
	}
}
bool parser_func_arglist(lines_t* lines, size_t* line, cbuild_sv_t* cline,
	arguments_t* args, cbuild_sb_t* decl) {
	cbuild_sb_t tmp = {0};
	cbuild_sv_trim_right(cline);
	cbuild_sv_trim_left(cline);
	// Start parsing argument list. This is two-level loop where outer one parses
	// pairs of '<type> <name>' which end with ',' or ')' and inner one parses
	// '<type>' out of separate tokens.
	char sep = '\0';
	bool noinsert_type = false;
	cbuild_sv_t token = sv_chop_token_with_separator(cline, &sep);
	if (cbuild_sv_cmp(token, cbuild_sv_from_lit("void")) != 0) {
		while (sep != ')') {
			while (sep != ',' && sep != ')') {
				if (!noinsert_type) {
					cbuild_sb_append_sv(&tmp, token);
					cbuild_sb_append(&tmp, ' ');
				}
				token = sv_chop_token_with_separator(cline, &sep);
				noinsert_type = false;
			}
			if (sep != ')') sep = '\0';
			if (tmp.size > 0) tmp.size--;
			while(*token.data == '*') {
				cbuild_sv_chop(&token, 1);
				cbuild_sb_append(&tmp, '*');
			}
			if (cbuild_sv_cmp(token, cbuild_sv_from_lit("...")) == 0) {
				cbuild_sb_append_cstr(&tmp, "variadics");
			}
			cbuild_da_append(args, ((argument_t){
						.type = sb_leak_as_sv(&tmp),
						.name = sv_dup(token),
					}));
			// Here we have cline fully-trimmed from left. Because any function
			// declaration line can end only with ',' or ');' this means that we can
			// directly check for `cline.size` (because we chop ',' and then trim
			// line) to determine if line need to be switched. For example, if we found
			// that size is greated that 0 it means that it either contains ');' or
			// other arguments. If it is 0 it means that declarations continues on
			// next line.
			if (cline->size == 0) {
				next_line(lines, line, cline, decl);
			}
			// Now we either have line starting from ')' (should not happen but this
			// is handled by this parser) or another argument (or arguments) in cline.
			noinsert_type = true;
		}
	}
	return true;
}
bool parser_func_decl(lines_t* lines, size_t line,
	cbuild_sv_t* ret, cbuild_sv_t* name, arguments_t* args, cbuild_sv_t* decl) {
	// Prepare working variables
	cbuild_sb_t tmp = {0};
	cbuild_sb_t d = {0};
	cbuild_sv_t cline = lines->data[line++];
	cbuild_sv_trim_right(&cline);
	cbuild_sb_append_sv(&d, cline);
	cbuild_sv_trim_left(&cline);
	// Strip attributes
	cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("CBUILD_ATTR_NORETURN("));
	if (cbuild_sv_prefix(cline, cbuild_sv_from_lit("CBUILD_ATTR_PRINTF(")) ||
		cbuild_sv_prefix(cline, cbuild_sv_from_lit("CBUILD_ATTR_DEPRECATED("))) {
		next_line(lines, &line, &cline, &d);
	}
	// Strip CBUILDDEF
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("CBUILDDEF "))) {
		cbuild_log_error("Function is not prefixed with CBUILDDEF: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	// Parse return value
	char sep = '\0';
	cbuild_sv_t token = sv_chop_token_with_separator(&cline, &sep);
	while (sep != '(') {
		cbuild_sb_append_sv(&tmp, token);
		cbuild_sb_append(&tmp, ' ');
		token = sv_chop_token_with_separator(&cline, &sep);
	}
	if (tmp.size > 0) tmp.size--;
	*ret = sb_leak_as_sv(&tmp);
	// Pare function name
	*name = sv_dup(token);
	// Parse function args
	if (!parser_func_arglist(lines, &line, &cline, args, &d)) return false;
	// Pass decl
	*decl = sb_leak_as_sv(&d);
	return true;
}
bool parser_var_decl(lines_t* lines, size_t line,
	cbuild_sv_t* type, cbuild_sv_t* name, cbuild_sv_t* decl) {
	cbuild_sb_t tmp = {0};
	cbuild_sv_t cline = lines->data[line++];
	if (cline.data[cline.size - 1] == '\\') cline.size--;
	cbuild_sv_trim_right(&cline);
	cbuild_sv_trim_left(&cline);
	if(!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("extern "))) {
		cbuild_log_error("Variable is not prefixed with extern: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	cbuild_sv_trim_left(&cline);
	char sep = '\0';
	cbuild_sv_t token = sv_chop_token_with_separator(&cline, &sep);
	while (sep != ';') {
		cbuild_sb_append_sv(&tmp, token);
		cbuild_sb_append(&tmp, ' ');
		token = sv_chop_token_with_separator(&cline, &sep);
	}
	if (tmp.size > 0) tmp.size--;
	*name = sv_dup(token);
	*type = sb_leak_as_sv(&tmp);
	int ret = asprintf(&decl->data, "extern "CBuildSVFmt" "CBuildSVFmt";",
		CBuildSVArg(*name), CBuildSVArg(*type));
	cbuild_assert(ret >= 0, "asprintf failed");
	decl->size = (size_t)ret;
	return true;
}
bool parser_define_decl(lines_t* lines, size_t line,
	cbuild_sv_t* name, cbuild_sv_t* decl) {
	cbuild_sv_t cline = lines->data[line++];
	if (cline.data[cline.size - 1] == '\\') cline.size--;
	cbuild_sv_trim_right(&cline);
	cbuild_sv_t cline_copy = cline;
	cbuild_sv_t def = sv_chop_token(&cline_copy);
	char sep = '\0';
	cbuild_sv_t nm = sv_chop_token_with_separator(&cline_copy, &sep);
	cbuild_sb_t d = {0};
	if (sep == '(') {
		// We can chop by ')' because we are parsing arglist.
		cbuild_sv_t args = cbuild_sv_chop_by_delim(&cline_copy, ')');
		cbuild_sb_appendf(&d, 
			CBuildSVFmt" "CBuildSVFmt"("CBuildSVFmt")",
			CBuildSVArg(def), CBuildSVArg(nm), CBuildSVArg(args));
	} else {
		cbuild_sb_appendf(&d,
			CBuildSVFmt" "CBuildSVFmt, CBuildSVArg(def), CBuildSVArg(nm));
	}
	*decl = cbuild_sv_from_sb(d);
	cbuild_sv_trim_left(&cline);
	if(!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("#define"))) {
		cbuild_log_error("Define is not prefixed with #define: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	cbuild_sv_trim_left(&cline);
	*name = sv_dup(sv_chop_token(&cline));
	return true;
}
bool parser_enum_values(lines_t* lines, size_t* line, cbuild_sv_t* cline,
	arguments_t* fields, cbuild_sb_t* decl) {
	next_line(lines, line, cline, decl);
	while (*cline->data != '}') { // Enum declaration ends when '}' is encountered
		cbuild_sv_t token = sv_chop_token(cline);
		cbuild_da_append(fields, ((argument_t){
					.type = cbuild_sv_from_parts(NULL, 0), // Enum values does not have individual types and just share overal type of an enum
					.name = sv_dup(token),
				}));
		next_line(lines, line, cline, decl);
	}
	return true;
}
// Needed to merge struct and typedef struct parsing.
// This assumes that cline is already in d.
bool parser_struct_fields(lines_t* lines, size_t* line, cbuild_sv_t* cline,
	arguments_t* fields, cbuild_sb_t* decl) {
	cbuild_sv_t token = {0};
	cbuild_sb_t tmp = {0};
	char sep = '\0';
	// Get next line
	next_line(lines, line, cline, decl);
	token = sv_chop_token_with_separator(cline, &sep);
	bool noinsert_type = false;
	while (*cline->data != '}') { // Struct declaration ends when '}' is encountered
		// Pars&e list of fields.
		while (sep != ';' && *cline->data != ':') {
			if (cbuild_sv_cmp(token, cbuild_sv_from_lit("{")) == 0) {
				if (cbuild_sv_prefix(cbuild_sv_from_sb(tmp), cbuild_sv_from_lit("struct"))) {
					// This is nested struct declaration.
					size_t last_idx = fields->size;
					if (!parser_struct_fields(lines, line, cline, fields, decl)) {
						return false;
					}
					// Parse field name
					cbuild_sv_chop(cline, 1); // Chop-out '}'
					token = sv_chop_token(cline);
					// Check for real type name
					cbuild_sb_t copy = {0};
					if (tmp.size >= sizeof("struct ")) {
						cbuild_sb_append_arr(&copy, tmp.data + (sizeof("struct ") - 1), tmp.size - (sizeof("struct ") - 1));
					}
					// Sometime anonymous structs are allowed and useful
					if (token.size > 0) {
						tmp.size = 0;
						cbuild_sv_t token_no_ptr = token;
						while(*token_no_ptr.data == '*') cbuild_sv_chop(&token_no_ptr, 1);
						for (size_t i = last_idx; i < fields->size; i++) {
							cbuild_sb_appendf(&tmp, CBuildSVFmt"."CBuildSVFmt,
								CBuildSVArg(token_no_ptr), CBuildSVArg(fields->data[i].name));
							free((void*)fields->data[i].name.data);
							fields->data[i].name = sb_leak_as_sv(&tmp);
						}
						if (copy.size > 0) {
							cbuild_sb_append_arr(&tmp, copy.data, copy.size);
						} else {
							cbuild_sb_append_cstr(&tmp, "struct { ... } ");
						}
					} else {
						cbuild_sb_clear(&tmp);
					}
					break;
				} else if (cbuild_sv_prefix(cbuild_sv_from_sb(tmp), cbuild_sv_from_lit("enum"))) {
					// This is nested enum declaration.
					if (!parser_enum_values(lines, line, cline, fields, decl)) {
						return false;
					}
					// Parse field name
					cbuild_sv_chop(cline, 1); // Chop-out '}'
					token = sv_chop_token(cline);
					// Check for real type name
					cbuild_sb_t copy = {0};
					if (tmp.size >= sizeof("enum ")) {
						cbuild_sb_append_arr(&copy, tmp.data + (sizeof("enum ") - 1), tmp.size - (sizeof("enum ") - 1));
					}
					// Sometime anonymous enums are allowed and useful
					if (token.size > 0) {
						tmp.size = 0;
						if (copy.size > 0) {
							cbuild_sb_append_arr(&tmp, copy.data, copy.size);
						} else {
							cbuild_sb_append_cstr(&tmp, "enum { ... } ");
						}
					} else {
						cbuild_sb_clear(&tmp);
					}
					break;
				} else if (cbuild_sv_prefix(cbuild_sv_from_sb(tmp), cbuild_sv_from_lit("union"))) {
					// This is nested union declaration.
					// Parse field
					size_t last_idx = fields->size;
					if (!parser_struct_fields(lines, line, cline, fields, decl)) {
						return false;
					}
					// Parse field name
					cbuild_sv_chop(cline, 1); // Chop-out '}'
					token = sv_chop_token(cline);
					// Check for real type name
					cbuild_sb_t copy = {0};
					if (tmp.size >= sizeof("union ")) {
						cbuild_sb_append_arr(&copy, tmp.data + (sizeof("union ") - 1), tmp.size - (sizeof("union ") - 1));
					}
					// In a lot of cases anonymous unions are useful
					if (token.size > 0) {
						tmp.size = 0;
						cbuild_sv_t token_no_ptr = token;
						while(*token_no_ptr.data == '*') cbuild_sv_chop(&token_no_ptr, 1);
						for (size_t i = last_idx; i < fields->size; i++) {
							cbuild_sb_appendf(&tmp, CBuildSVFmt"."CBuildSVFmt,
								CBuildSVArg(token_no_ptr), CBuildSVArg(fields->data[i].name));
							free((void*)fields->data[i].name.data);
							fields->data[i].name = sb_leak_as_sv(&tmp);
						}
						if (copy.size > 0) {
							cbuild_sb_append_arr(&tmp, copy.data, copy.size);
						} else {
							cbuild_sb_append_cstr(&tmp, "struct { ... } ");
						}
					} else {
						cbuild_sb_clear(&tmp);
					}
					break;
				} else {
					CBUILD_UNREACHABLE("Invalid composite detected: "CBuildSBFmt".", 
						CBuildSBArg(tmp));
				}
			}
			if (!noinsert_type) {
				cbuild_sb_append_sv(&tmp, token);
				cbuild_sb_append(&tmp, ' ');
			}
			token = sv_chop_token_with_separator(cline, &sep);
			noinsert_type = false;
		}
		if (tmp.size > 0) tmp.size--;
		while(*token.data == '*') {
			cbuild_sv_chop(&token, 1);
			cbuild_sb_append(&tmp, '*');
		}
		// INFO: This fixes type for bitfields
		// NOTE: This checks if 'type' string builder is not-empty because for anonymous
		// bitfields ('type : bitcount') parser will put 'type' into name field.
		if (tmp.size > 0 && *cline->data == ':') {
			cbuild_sv_chop(cline, 1);
			cbuild_sv_t count = sv_chop_token(cline);
			cbuild_sb_appendf(&tmp, ":"CBuildSVFmt, CBuildSVArg(count));
		}
		// INFO: This fixes type for sized arrays
		if (token.size > 0 && cbuild_sv_contains(token, '[')) {
			cbuild_sv_t token_copy = token;
			token = cbuild_sv_chop_by_delim(&token_copy, '[');
			cbuild_sb_appendf(&tmp, "["CBuildSVFmt, CBuildSVArg(token_copy));
		}
		if (tmp.size > 0 && token.size > 0) {
			cbuild_da_append(fields, ((argument_t){
						.type = sb_leak_as_sv(&tmp),
						.name = sv_dup(token),
					}));
		}
		// Get next line
		next_line(lines, line, cline, decl);
		if (sep != '}') sep = '\0';
		noinsert_type = true;
	}
	return true;
}
bool parser_struct_decl(lines_t* lines, size_t line,
	cbuild_sv_t* name, arguments_t* fields, cbuild_sv_t* decl) {
	cbuild_sb_t d = {0};
	cbuild_sv_t cline = lines->data[line++];
	cbuild_sv_trim_right(&cline);
	cbuild_sb_append_sv(&d, cline);
	cbuild_sv_trim_left(&cline);
	// Parse 'struct <name> {'
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("struct "))) {
		cbuild_log_error("Structure is not prefixed with struct: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	*name = sv_dup(sv_chop_token(&cline));
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("{"))) {
		cbuild_log_error("Structure is not prefixed with {.: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	// Parse fields
	if (!parser_struct_fields(lines, &line, &cline, fields, &d)) {
		return false;
	}
	// Cline here have '};' at the start.
	if (!cbuild_sv_prefix(cline, cbuild_sv_from_lit("};"))) {
		cbuild_log_error("Structure does not end with };: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	*decl = cbuild_sv_from_sb(d);
	return true;
}
bool parser_enum_decl(lines_t* lines, size_t line,
	cbuild_sv_t* name, arguments_t* values, cbuild_sv_t* decl) {
	cbuild_sb_t d = {0};
	cbuild_sv_t cline = lines->data[line++];
	cbuild_sv_trim_right(&cline);
	cbuild_sb_append_sv(&d, cline);
	cbuild_sv_trim_left(&cline);
	// Parse 'enum <name> {'
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("enum "))) {
		cbuild_log_error("Enumeration is not prefixed with enum: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	*name = sv_dup(sv_chop_token(&cline));
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("{"))) {
		cbuild_log_error("Enumeration is not prefixed with {: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	// Parse fields
	if (!parser_enum_values(lines, &line, &cline, values, &d)) {
		return false;
	}
	// Cline here have '};' at the start.
	if (!cbuild_sv_prefix(cline, cbuild_sv_from_lit("};"))) {
		cbuild_log_error("Enumeration does not end with };: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	*decl = cbuild_sv_from_sb(d);
	return true;
}
bool parser_typedef_decl(lines_t* lines, size_t line,
	cbuild_sv_t* typename, cbuild_sv_t* name, cbuild_sv_t* decl) {
	cbuild_sb_t tmp = {0};
	cbuild_sv_t cline = lines->data[line++];
	if (cline.data[cline.size - 1] == '\\') cline.size--;
	cbuild_sv_trim_right(&cline);
	cbuild_sv_trim_left(&cline);
	if(!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("typedef "))) {
		cbuild_log_error("Typedef is not prefixed with typedef: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	cbuild_sv_trim_left(&cline);
	char sep = '\0';
	cbuild_sv_t token = sv_chop_token_with_separator(&cline, &sep);
	while (sep != ';') {
		cbuild_sb_append_sv(&tmp, token);
		cbuild_sb_append(&tmp, ' ');
		token = sv_chop_token_with_separator(&cline, &sep);
	}
	if (tmp.size > 0) tmp.size--;
	*name = sv_dup(token);
	*typename = sb_leak_as_sv(&tmp);
	int ret = asprintf(&decl->data, "typename "CBuildSVFmt" "CBuildSVFmt, 
		CBuildSVArg(*name), CBuildSVArg(*typename));
	cbuild_assert(ret >= 0, "asprintf failed");
	decl->size = (size_t)ret;
	return true;
}
bool parser_typedef_struct_decl(lines_t* lines, size_t line,
	cbuild_sv_t* typename, cbuild_sv_t* name, arguments_t* fields, cbuild_sv_t* decl) {
	cbuild_sb_t d = {0};
	cbuild_sv_t cline = lines->data[line++];
	cbuild_sv_trim_right(&cline);
	cbuild_sb_append_sv(&d, cline);
	cbuild_sv_trim_left(&cline);
	// Parse 'typedef struct <name> {'
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("typedef "))) {
		cbuild_log_error("Structure typedef is not prefixed with struct"CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("struct "))) {
		cbuild_log_error("Structure typedef is not prefixed with struct"CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	cbuild_sv_trim_left(&cline);
	if (*cline.data != '{') {
		*typename = sv_dup(sv_chop_token(&cline));
	}
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("{"))) {
		cbuild_log_error("Structure typedef is not prefixed with {: "CBuildSVFmt,
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	// Parse fields
	if (!parser_struct_fields(lines, &line, &cline, fields, &d)) {
		return false;
	}
	// Cline here have '}' at the start.
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("}"))) {
		cbuild_log_error("Structure typedef does not end with }: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	char sep = '\0';
	*name = sv_dup(sv_chop_token_with_separator(&cline, &sep));
	// Cline here have '}' at the start.
	if (sep != ';') {
		cbuild_log_error("Structure typedef does not end with ;: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	*decl = cbuild_sv_from_sb(d);
	return true;
}
bool parser_typedef_enum_decl(lines_t* lines, size_t line,
	cbuild_sv_t* typename, cbuild_sv_t* name, arguments_t* values, cbuild_sv_t* decl) {
	cbuild_sb_t d = {0};
	cbuild_sv_t cline = lines->data[line++];
	cbuild_sv_trim_right(&cline);
	cbuild_sb_append_sv(&d, cline);
	cbuild_sv_trim_left(&cline);
	// Parse 'typedef enum <name> {'
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("typedef "))) {
		cbuild_log_error("Enumeration typedef is not prefixed with typedef: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("enum "))) {
		cbuild_log_error("Enumeration typedef is not prefixed with enum: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	cbuild_sv_trim_left(&cline);
	if (*cline.data != '{') {
		*typename = sv_dup(sv_chop_token(&cline));
	}
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("{"))) {
		cbuild_log_error("Enumeration typedef is not prefixed with {: "CBuildSVFmt,
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	// Parse fields
	if (!parser_enum_values(lines, &line, &cline, values, &d)) {
		return false;
	}
	// Cline here have '}' at the start.
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("}"))) {
		cbuild_log_error("Enumeration typedef does not end with }: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	char sep = '\0';
	*name = sv_dup(sv_chop_token_with_separator(&cline, &sep));
	// Cline here have '}' at the start.
	if (sep != ';') {
		cbuild_log_error("Enumeration typedef does not end with ;: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	*decl = cbuild_sv_from_sb(d);
	return true;
}
bool parser_typedef_func_decl(lines_t* lines, size_t line,
	cbuild_sv_t* ret, cbuild_sv_t* name, arguments_t* args, cbuild_sv_t* decl) {
	// Prepare working variables
	cbuild_sb_t tmp = {0};
	cbuild_sb_t d = {0};
	cbuild_sv_t cline = lines->data[line++];
	cbuild_sv_trim_right(&cline);
	cbuild_sb_append_sv(&d, cline);
	cbuild_sv_trim_left(&cline);
	// Strip typedef
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("typedef "))) {
		cbuild_log_error("Function is not prefixed: "CBuildSVFmt".",
			CBuildSVArg(lines->data[line - 1]));
		return false;
	}
	// Parse return value
	char sep = '\0';
	cbuild_sv_t token = sv_chop_token_with_separator(&cline, &sep);
	while (sep != '(') {
		cbuild_sb_append_sv(&tmp, token);
		cbuild_sb_append(&tmp, ' ');
		token = sv_chop_token_with_separator(&cline, &sep);
	}
	if (tmp.size > 0) tmp.size--;
	*ret = sb_leak_as_sv(&tmp);
	// Parse function name
	cbuild_sv_chop(&cline, 1); // Chop '*'	
	*name = sv_dup(sv_chop_token(&cline));
	cbuild_sv_chop(&cline, 1); // Chop '(' which starts arguments.	
	// Parse function args
	if (!parser_func_arglist(lines, &line, &cline, args, &d)) return false;
	// Pass decl
	*decl = cbuild_sv_from_sb(d);
	return true;
}
