// C declaration parser for CBuild.
// This file can parse declarations from CBuild, not C declarations in general.
// License: `GPL-3.0-or-later`.

// TODO: Fix that note
declaration_type_t extract_decl_type(lines_t lines, size_t line) {
	cbuild_sv_t cline = lines.data[line];
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
// TODO: decl is single-line only
bool parse_func_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** ret, char** name, arguments_t* args, char** decl) {
	// Prepare working variables
	cbuild_sb_t tmp = {0};
	cbuild_sb_t d = {0};
	cbuild_sv_t cline = lines.data[line++];
	cbuild_sv_trim_right(&cline);
	cbuild_sb_append_sv(&d, cline);
	cbuild_sv_trim_left(&cline);
	// Strip attributes
	cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("CBUILD_ATTR_NORETURN("));
	if (cbuild_sv_prefix(cline, cbuild_sv_from_lit("CBUILD_ATTR_PRINTF(")) ||
		cbuild_sv_prefix(cline, cbuild_sv_from_lit("CBUILD_ATTR_DEPRECATED("))) {
		cline = lines.data[line++];
		cbuild_sv_trim_right(&cline);
		cbuild_sb_append_sv(&d, cline);
		cbuild_sv_trim_left(&cline);
	}
	// Strip CBUILDDEF
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("CBUILDDEF "))) {
		cbuild_log_error("Function is not prefixed!");
		return false;
	}
	// Parse return value
	char sep = '\0';
	cbuild_sv_t token = sv_chop_token_with_separator(&cline, &sep);
	while (sep != '(') {
		cbuild_sb_appendf(&tmp, CBuildSVFmt" ", CBuildSVArg(token));
		token = sv_chop_token_with_separator(&cline, &sep);
	}
	tmp.size--;
	cbuild_sb_append_null(&tmp);
	*ret = cbuild_arena_strdup(allocator, tmp.data);
	tmp.size = 0;
	// Pare function name
	*name = cbuild_arena_sprintf(allocator, CBuildSVFmt, CBuildSVArg(token));
	// Get argument count
	// C uses commas only as argument delimiter in function declaration. Also such
	// declaration should end with ';' (at least this is true for CBuild).
	size_t num_args = 0;
	for (size_t i = line - 1; i < lines.size; i++) {
		num_args += cbuild_sv_count(lines.data[i], ',');
		// This checks if current line does not end with ';'. Because if it ends this
		// means that it is the last line of this declaration.
		if (lines.data[i].data[lines.data[i].size - 1] == ';') break;
	}
	// Last argument does not have comma after. If function have no arguments,
	// this will allocate 1 argument, which is harmless.
	num_args++;	
	args->data = cbuild_arena_malloc(allocator, sizeof(*args->data) * num_args);
	// Start parsing argument list. This is two-level loop where outer one parses
	// pairs of '<type> <name>' which end with ',' or ')' and inner one parses
	// '<type>' out of separate tokens.
	sep = '\0';
	token = sv_chop_token_with_separator(&cline, &sep);
	if (cbuild_sv_cmp(token, cbuild_sv_from_lit("void")) != 0) {
		size_t arg = 0;
		while (sep != ')') {
			while (sep != ',' && sep != ')') {
				cbuild_sb_append_sv(&tmp, token);
				cbuild_sb_append(&tmp, ' ');
				token = sv_chop_token_with_separator(&cline, &sep);
			}
			cbuild_sv_trim_left(&cline); // Trim line again
			tmp.size--;
			while (*token.data == '*') {
				cbuild_sb_append_sv(&tmp, cbuild_sv_chop(&token, 1));
			}
			cbuild_sb_append_null(&tmp);
			args->data[arg].type = cbuild_arena_strdup(allocator, tmp.data);
			tmp.size = 0;
			args->data[arg].name = 
				cbuild_arena_sprintf(allocator, CBuildSVFmt, CBuildSVArg(token));
			arg++;
			// Here we have cline fully-trimmed from left. Because any function
			// declaration line can end only with ',' or ');' this means that we can
			// directly check for `cline.size` (because we chop ',' and then trim
			// line) to determine if line need to be switched. For example, if we found
			// that size is greated that 0 it means that it either contains ');' or
			// other arguments. If it is 0 it means that declarations continues on
			// next line.
			if (cline.size == 0) {
				cbuild_sb_append_cstr(&d, "\n");
				cline = lines.data[line++];
				cbuild_sv_trim_right(&cline);
				cbuild_sb_append_sv(&d, cline);
				cbuild_sv_trim_left(&cline);
			}
			// Now we either have line starting from ')' (should not happen but this
			// is handled by this parser) or another argument (or arguments) in cline.
		}
		args->size = arg;
	}
	// Pass decl
	cbuild_sb_append_null(&d);
	*decl = cbuild_arena_strdup(allocator, d.data);
	cbuild_sb_clear(&tmp);
	cbuild_sb_clear(&d);
	return true;
}
bool parse_var_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** type, char** name, char** decl) {
	cbuild_sb_t tmp = {0};
	cbuild_sv_t cline = lines.data[line++];
	if (cline.data[cline.size - 1] == '\\') cline.size--;
	cbuild_sv_trim_right(&cline);
	*decl = cbuild_arena_sprintf(allocator, CBuildSVFmt, CBuildSVArg(cline));
	cbuild_sv_trim_left(&cline);
	if(!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("extern "))) {
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
	*name = cbuild_arena_sprintf(allocator, CBuildSVFmt, CBuildSVArg(token));
	tmp.size--;
	cbuild_sb_append_null(&tmp);
	*type = cbuild_arena_strdup(allocator, tmp.data);
	*decl = cbuild_arena_sprintf(allocator,"extern %s "CBuildSVFmt";",
		tmp.data, CBuildSVArg(token));
	cbuild_sb_clear(&tmp);
	return true;
}
bool parse_define_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** name, char** decl) {
	cbuild_sv_t cline = lines.data[line++];
	if (cline.data[cline.size - 1] == '\\') cline.size--;
	cbuild_sv_trim_right(&cline);
	cbuild_sv_t d = cline;
	cbuild_sv_t def = sv_chop_token(&d);
	char sep = '\0';
	cbuild_sv_t nm = sv_chop_token_with_separator(&d, &sep);
	if (sep == '(') {
		// We can chop by ')' because we are parsing arglist.
		cbuild_sv_t args = cbuild_sv_chop_by_delim(&d, ')');
		*decl = cbuild_arena_sprintf(allocator, 
			CBuildSVFmt" "CBuildSVFmt"("CBuildSVFmt")",
			CBuildSVArg(def), CBuildSVArg(nm), CBuildSVArg(args));
	} else {
		*decl = cbuild_arena_sprintf(allocator, 
			CBuildSVFmt" "CBuildSVFmt, CBuildSVArg(def), CBuildSVArg(nm));
	}
	cbuild_sv_trim_left(&cline);
	if(!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("#define"))) {
		return false;
	}
	cbuild_sv_trim_left(&cline);
	cbuild_sv_t n = sv_chop_token(&cline);
	*name = cbuild_arena_sprintf(allocator, CBuildSVFmt, CBuildSVArg(n));
	return true;
}
bool parse_typedef_func_decl(lines_t lines, size_t line, cbuild_arena_t* allocator,
	char** ret, char** name, arguments_t* args, char** decl) {
	// Prepare working variables
	cbuild_sb_t tmp = {0};
	cbuild_sb_t d = {0};
	cbuild_sv_t cline = lines.data[line++];
	cbuild_sv_trim_right(&cline);
	cbuild_sb_append_sv(&d, cline);
	cbuild_sv_trim_left(&cline);
	// Strip typedef
	if (!cbuild_sv_chop_prefix(&cline, cbuild_sv_from_lit("typedef "))) {
		cbuild_log_error("Function is not prefixed!");
		return false;
	}
	// Parse return value
	char sep = '\0';
	cbuild_sv_t token = sv_chop_token_with_separator(&cline, &sep);
	while (sep != '(') {
		cbuild_sb_appendf(&tmp, CBuildSVFmt" ", CBuildSVArg(token));
		token = sv_chop_token_with_separator(&cline, &sep);
	}
	cbuild_sb_append_sv(&tmp, token);
	cbuild_sb_append_null(&tmp);
	*ret = cbuild_arena_strdup(allocator, tmp.data);
	tmp.size = 0;
	// Pare function name
	cbuild_sv_chop(&cline, 1); // Chop '*'	
	token = sv_chop_token(&cline);
	*name = cbuild_arena_sprintf(allocator, CBuildSVFmt, CBuildSVArg(token));
	cbuild_sv_chop(&cline, 1); // Chop '(' which starts arguments.	
	// Get argument count
	// C uses commas only as argument delimiter in function declaration. Also such
	// declaration should end with ';' (at least this is true for CBuild).
	size_t num_args = 0;
	for (size_t i = line - 1;
		// This line both checks if there are still some lines and if current line
		// does not end with ';'. Because if it ends this means that it is the last
		// line of this declaration.
		i < lines.size && lines.data[i].data[lines.data[i].size - 1] != ';';
		i++) {
		num_args += cbuild_sv_count(lines.data[i], ',');
	}
	// Last argument does not have comma after. If function have no arguments,
	// this will allocate 1 argument, which is harmless.
	num_args++;	
	args->data = cbuild_arena_malloc(allocator, sizeof(*args->data) * num_args);
	// Start parsing argument list. This is two-level loop where outer one parses
	// pairs of '<type> <name>' which end with ',' or ')' and inner one parses
	// '<type>' out of separate tokens.
	sep = '\0';
	token = sv_chop_token_with_separator(&cline, &sep);
	if (cbuild_sv_cmp(token, cbuild_sv_from_lit("void")) != 0) {
		size_t arg = 0;
		while (sep != ')') {
			while (sep != ',' && sep != ')') {
				cbuild_sb_append_sv(&tmp, token);
				cbuild_sb_append(&tmp, ' ');
				token = sv_chop_token(&cline);
			}
			cbuild_sv_trim_left(&cline); // Trim line again
			tmp.size--;
			while (*token.data == '*') {
				cbuild_sb_append_sv(&tmp, cbuild_sv_chop(&token, 1));
			}
			cbuild_sb_append_null(&tmp);
			args->data[arg].type = cbuild_arena_strdup(allocator, tmp.data);
			tmp.size = 0;
			args->data[arg].name = 
				cbuild_arena_sprintf(allocator, CBuildSVFmt, CBuildSVArg(token));
			arg++;
			// Here we have cline fully-trimmed from left. Because any function
			// declaration line can end only with ',' or ');' this means that we can
			// directly check for `cline.size` (because we chop ',' and then trim
			// line) to determine if line need to be switched. For example, if we found
			// that size is greated that 0 it means that it either contains ');' or
			// other arguments. If it is 0 it means that declarations continues on
			// next line.
			if (cline.size == 0) {
				cbuild_sb_append_cstr(&d, "\n");
				cline = lines.data[line++];
				cbuild_sv_trim_right(&cline);
				cbuild_sb_append_sv(&d, cline);
				cbuild_sv_trim_left(&cline);
			}
			// Now we either have line starting from ')' (should not happen but this
			// is handled by this parser) or another argument (or arguments) in cline.
		}
		args->size = arg;
	}
	// Pass decl
	cbuild_sb_append_null(&d);
	*decl = cbuild_arena_strdup(allocator, d.data);
	cbuild_sb_clear(&tmp);
	cbuild_sb_clear(&d);
	return true;
}
