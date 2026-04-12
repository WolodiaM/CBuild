// Wiki and code doc generator.
// License: `GPL-3.0-or-later`.
// ----------------------------------------
// Shared types and declarations
// ----------------------------------------
#define CBUILD_PROFILER
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "wikimk.h"
// ----------------------------------------
// Includes
// ----------------------------------------
#include "sys/socket.h"
#include "netinet/in.h"
#define CBUILD_IMPLEMENTATION
#include "../cbuild.split.h"
// ----------------------------------------
// Config
// ----------------------------------------
#define WIKI_NAME     "CBuild wiki"
#define WIKI_AUTHOR   "WolodiaM"
#define WIKI_LICENSE  "GFDL-1.3-or-later"
#define SITE_BASE_URL "/CBuild"
// Paths
#define CODE_DOC_SRC     "src" // Prefix
#define CODE_DOC_OUT     "wiki/doc"
#define CODE_DOC_SYMBOLS "wiki/doc/symbols.md"
#define WIKIMK_TEMPLATE  "wikimk/templates"
#define WIKIMK_FILTERS   "wikimk/filters"
#define WIKIMK_ERRORS    "wikimk/errors"
#define WIKIMK_STYLES    "wikimk/styles"
#define WIKIMK_SCRIPTS   "wikimk/scripts"
#define WIKI_SRC         "wiki"
#define WIKI_OUT         "build/wiki"
#define CHANGELOG_SRC    "changelog"
#define CHANGELOG_DST    "wiki/changelog"
// #define DEBUG_PARSER
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
	void (*decl_name)(cbuild_sb_t* dst, cbuild_sv_t name, const char* src, size_t line);
	void (*decl_code)(cbuild_sb_t*dst, cbuild_sv_t decl_raw);
	// Formatting of parsed data
	void (*parsed_arg_in_list)(cbuild_sb_t* dst, argument_t arg);
	void (*parsed_field_in_list)(cbuild_sb_t* dst, argument_t arg);
	void (*parsed_enum_value_in_list)(cbuild_sb_t* dst, argument_t arg);
	void (*parsed_ret)(cbuild_sb_t* dst, cbuild_sv_t text);
	void (*parsed_arg_standalone)(cbuild_sb_t* dst, argument_t arg);
	void (*parsed_field_standalone)(cbuild_sb_t* dst, argument_t arg);
	void (*parsed_enum_value_standalone)(cbuild_sb_t* dst, argument_t arg);
	// Symbol table formatting
	void (*symtab_entry)(cbuild_sb_t* dst, const char* symbol, const char* file);
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
		cbuild_sv_t* typename, cbuild_sv_t* name, cbuild_sv_t* decl);
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
bool docgen_generate_symbols(const char* output, symbols_t* symbols);
bool wikimk_generate_nav_html(const char* output, const char* src);
bool wikimk_build_error(int code);
bool wikimk_dir_walk(cbuild_dir_walk_func_args_t args);
bool http_serve_dir(const char* dir, uint16_t port);
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
	TEMPLATE.parsed_field_in_list = cbuild_dlib_get_sym(TEMPLATE.lib, "template_parsed_field_in_list");
	TEMPLATE.parsed_enum_value_in_list = cbuild_dlib_get_sym(TEMPLATE.lib, "template_parsed_enum_value_in_list");
	TEMPLATE.parsed_ret = cbuild_dlib_get_sym(TEMPLATE.lib, "template_parsed_ret");
	TEMPLATE.parsed_arg_standalone = cbuild_dlib_get_sym(TEMPLATE.lib, "template_parsed_arg_standalone");
	TEMPLATE.parsed_field_standalone = cbuild_dlib_get_sym(TEMPLATE.lib, "template_parsed_field_standalone");
	TEMPLATE.parsed_enum_value_standalone = cbuild_dlib_get_sym(TEMPLATE.lib, "template_parsed_enum_value_standalone");
	TEMPLATE.symtab_entry = cbuild_dlib_get_sym(TEMPLATE.lib, "template_symtab_entry");
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
void atexit_handler(void) {
	cbuild_temp_profiler();
}
cbuild_proclist_t procs = {0};
bool main_prelude(void) {
	if (!load_template()) return false;
	if (!load_parser()) return false;
	return true;
}
bool main_epilogue(void) {
	cbuild_dlib_close(PARSER.lib);
	cbuild_dlib_close(TEMPLATE.lib);
	return true;
}
bool main_docgen(void) {
	cbuild_log_info("Processing doc comments from '%s' into '%s.",
		CODE_DOC_SRC, CODE_DOC_OUT);
	if (cbuild_dir_check(CODE_DOC_OUT) && !cbuild_dir_remove(CODE_DOC_OUT)) {
		cbuild_log_error("Could not clean build folder '"CODE_DOC_OUT"'.");
		return false;
	}
	if (!cbuild_dir_check(CODE_DOC_SRC)) {
		cbuild_log_error("Directory "CODE_DOC_SRC" is code doc source but it does not exist.");
		return false;
	}
	if (!cbuild_dir_check(CODE_DOC_OUT)) {
		if (!cbuild_dir_create(CODE_DOC_OUT)) return false;
	}
	symbols_t symbols = {0};
	cbuild_arena_base_malloc(&symbols.arena, CBUILD_TEMP_ARENA_SIZE);
	if (!cbuild_dir_walk(CODE_DOC_SRC, docgen_dir_walk, .context = &symbols)) {
		return false;
	}
	if (!docgen_generate_symbols(CODE_DOC_SYMBOLS, &symbols)) {
		return false;
	}
	cbuild_sb_t dirname = {0};
	cbuild_sb_append_cstr(&dirname, "Code Doc");
	if (!cbuild_file_write(CODE_DOC_OUT"/.dirname", &dirname)) return false;
	cbuild_sb_clear(&dirname);
	cbuild_arena_base_free(&symbols.arena);
	return true;
}
bool main_wikimk(void) {
	if (cbuild_dir_check(WIKI_OUT) && !cbuild_dir_remove(WIKI_OUT)) {
		cbuild_log_error("Could not clean build folder '"WIKI_OUT"'.");
		return false;
	}
	// TODO: Extract this somehow. Probably as a list of external of external files
	cbuild_log_info("Copying changelog from '%s' to '%s'.", CHANGELOG_SRC, CHANGELOG_DST);
	if (cbuild_dir_check(CHANGELOG_DST) && !cbuild_dir_remove(CHANGELOG_DST)) {
		cbuild_log_error("Could not clean changelog output '"CHANGELOG_DST"'.");
		return false;
	}
	if (!cbuild_dir_copy(CHANGELOG_SRC, CHANGELOG_DST)) return false;
	cbuild_sb_t dirname = {0};
	cbuild_sb_append_cstr(&dirname, "Changelog");
	if (!cbuild_file_write(CHANGELOG_DST"/.dirname", &dirname)) return false;
	cbuild_sb_clear(&dirname);
	cbuild_log_info("Processing wiki from '%s' into '%s.", WIKI_SRC, WIKI_OUT);
	if (!cbuild_dir_check(WIKI_SRC)) {
		cbuild_log_error("Directory "WIKI_SRC" is wiki source but it does not exist.");
		return false;
	}
	if (!wikimk_generate_nav_html(WIKIMK_TEMPLATE"/nav.html", WIKI_SRC)) {
		return false;
	}
	if (!cbuild_dir_walk(WIKI_SRC, wikimk_dir_walk, .context = WIKI_OUT)) {
		return false;
	}
	if (!wikimk_build_error(404)) return false;
	if (!wikimk_build_error(405)) return false;
	if (!cbuild_procs_wait(procs)) return false;
	if (!cbuild_dir_copy(WIKIMK_STYLES, WIKI_OUT"/styles")) {
		cbuild_log_error("Could not copy styles.");
		return false;
	}
	if (!cbuild_dir_copy(WIKIMK_SCRIPTS, WIKI_OUT"/scripts")) {
		cbuild_log_error("Could not copy scripts.");
		return false;
	}
	return true;
}
bool main_serve(uint16_t port) {
	cbuild_log_info("Serving local copy of wiki on localhost:%d", port);
	printf("http://localhost:%d/\n", port);
	if (!http_serve_dir(WIKI_OUT, port)) return false;
	return true;
}
void main_help(const char* progname) {
	printf("Usage: %s <subcommand> [args]\n", progname);
	printf("Subcommands:\n");
	printf("  - help            Print this message.\n");
	printf("  - docgen          Run DocGen part.\n");
	printf("  - wikimk          Run WikiMK part.\n");
	printf("  - build           Run DocGen and then WikiMK.\n");
	printf("  - serve <port>    Serve file on localhost on given port. Port should be an integer in range of 1 to 65535.\n");
}
int main(int argc, char** argv) {
	atexit(atexit_handler);
	if (!main_prelude()) return 1;
	const char* progname = cbuild_shift(argv, argc);
	if (argc == 0) {
		printf("ERROR: No subcommand provided.\n");
		main_help(progname);
		return 1;
	}
	const char* command = cbuild_shift(argv, argc);
	if (strcmp(command, "help") == 0) {
		main_help(progname);
	} else if (strcmp(command, "docgen") == 0) {
		if (!main_docgen()) return 1;
	} else if (strcmp(command, "wikimk") == 0) {
		if (!main_wikimk()) return 1;
	} else if (strcmp(command, "build") == 0) {
		if (!main_docgen()) return 1;
		if (!main_wikimk()) return 1;
	} else if (strcmp(command, "serve") == 0) {
		if (argc == 0) {
			printf("ERROR: Subcommand 'serve' requires an argument.\n");
			main_help(progname);
			return 1;
		}
		const char* arg = cbuild_shift(argv, argc);
		char* num_end = NULL;
		long port = strtol(arg, &num_end, 10);
		if (port < 1 || port > UINT16_MAX || *num_end != '\0') {
			printf("ERROR: Invalid port provided to subcommand 'serve'.\n");
			main_help(progname);
			return 1;
		}
		if (!main_serve((uint16_t)port)) return 1;
	} else {
		printf("ERROR: Invalid subcommand provided.\n");
		main_help(progname);
		return 1;
	}
	if (!main_epilogue()) return 1;
	return 0;
}
bool push_symbol(symbols_t* symbol_table, cbuild_sv_t name, const char* filename) {
	size_t checkpoint = cbuild_arena_checkpoint(&symbol_table->arena);
	char* str = cbuild_arena_malloc(&symbol_table->arena, name.size + 1);
	memcpy(str, name.data, name.size);
	str[name.size] = '\0';
	cbuild_da_foreach(*symbol_table, symbol) {
		if (strcmp(symbol->symbol_name, str) == 0) {
			cbuild_arena_reset(&symbol_table->arena, checkpoint);
			return false;
		}
	}
	cbuild_da_append(symbol_table, ((symbol_t){
				.symbol_name = str,
				.filename = cbuild_arena_strdup(&symbol_table->arena, filename),
			}));
	return true;
}
// ----------------------------------------
// WikiMK
// ----------------------------------------
void wikimk_cmd_append_pandoc_base_and_edit(cbuild_cmd_t* cmd, const char* path) {
	cbuild_cmd_append(cmd, "pandoc");
	// Base args
	cbuild_cmd_append_many(cmd,
		"-f", "markdown",
		"-t", "html5");
	cbuild_cmd_append_many(cmd,
		"-M", "name:" WIKI_NAME,
		"-M", "author:" WIKI_AUTHOR,
		"-M", "license:" WIKI_LICENSE,
				"-M", "BASE_URL:"SITE_BASE_URL);
	cbuild_cmd_append_many(cmd,
		"--lua-filter="WIKIMK_FILTERS"/codeblock-include-file.lua",
		"--lua-filter="WIKIMK_FILTERS"/codeblock-include-dir-tree.lua",
		"--lua-filter="WIKIMK_FILTERS"/url-transform-doc-ref.lua");
	// Edit URL
	cbuild_sb_t edit_url = {0};
	cbuild_sb_append_cstr(&edit_url, "EDIT-URL:");
	TEMPLATE.code_edit(&edit_url, path, 0);
	cbuild_sb_append_null(&edit_url);
	cbuild_cmd_append_many(cmd, "-M", cbuild_temp_strdup(edit_url.data));
	cbuild_sb_clear(&edit_url);
	// Edit filepath
	char* edit_file = cbuild_temp_sprintf("EDIT-FILENAME:/%s", path);
	cbuild_cmd_append_many(cmd, "-M", edit_file);
}
bool wikimk_build_error(int code) {
	cbuild_cmd_t cmd = {0};
	const char* md_path = cbuild_temp_sprintf(WIKIMK_ERRORS"/%d.md", code);
	const char* html_path = cbuild_temp_sprintf(WIKI_OUT"/%d.html", code);
	wikimk_cmd_append_pandoc_base_and_edit(&cmd, md_path);
	cbuild_cmd_append_many(&cmd, "--template", WIKIMK_TEMPLATE"/template.html");
	cbuild_cmd_append(&cmd, md_path);
	cbuild_cmd_append_many(&cmd, "-o", html_path);
	cbuild_log(CBUILD_LOG_TRACE,
		"Generating error page for %d into '/%s'.", code, html_path);
	if (!cbuild_cmd_run(&cmd, .procs = &procs)) return false;
	// Cleanup
	cbuild_cmd_clear(&cmd);
	cbuild_temp_reset(0);
	return true;
}
bool wikimk_dir_walk(cbuild_dir_walk_func_args_t args) {
	cbuild_sv_t path = cbuild_sv_from_cstr(args.path);
	// TODO: Allow to register some custom 'special cases'?
	if (cbuild_sv_cmp(path, cbuild_sv_from_lit(WIKI_SRC"/index.cfg")) == 0) {
		// Get redirect destination
		cbuild_sb_t file_content_sb = {0};
		if (!cbuild_file_read(args.path, &file_content_sb)) return false;
		cbuild_sv_t file_content = cbuild_sv_from_sb(file_content_sb);
		cbuild_sv_t name = cbuild_sv_chop_by_delim(&file_content, '\n');
		cbuild_sv_trim(&name);
		// Insert redirect destination
		cbuild_cmd_t cmd = {0};
		wikimk_cmd_append_pandoc_base_and_edit(&cmd, args.path);
		cbuild_cmd_append_many(&cmd, "--template", WIKIMK_TEMPLATE"/redirect.html");
		const char* redirect =
			cbuild_temp_sprintf("REDIRECT:"CBuildSVFmt, CBuildSVArg(name));
		cbuild_cmd_append_many(&cmd, "-M", redirect);
		cbuild_cmd_append_many(&cmd, "-o", WIKI_OUT"/index.html");
		cbuild_cmd_append(&cmd, "/dev/null"); // Some input file is needed, but no input is really needed
		// Run command
		cbuild_log(CBUILD_LOG_TRACE,
			"Generating index.html redirect to '/"CBuildSVFmt"'.", CBuildSVArg(name));
		if (!cbuild_cmd_run(&cmd, .procs = &procs)) return false;
		// Cleanup
		cbuild_cmd_clear(&cmd);
		cbuild_sb_clear(&file_content_sb);
		cbuild_temp_reset(0);
		return true;
	}
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("changelog/changelog.txt"))) {
		cbuild_cmd_t cmd = {0};
		wikimk_cmd_append_pandoc_base_and_edit(&cmd, args.path);
		cbuild_cmd_append_many(&cmd, "--template", WIKIMK_TEMPLATE"/template.html");
		cbuild_cmd_append(&cmd, args.path);
		// Output file
		path.size -= 4; // Chop '.txt'
		cbuild_sv_chop_prefix(&path, cbuild_sv_from_lit(WIKI_SRC"/"));
		char* out = cbuild_temp_sprintf(WIKI_OUT"/"CBuildSVFmt".html", CBuildSVArg(path));
		cbuild_cmd_append_many(&cmd, "-o", out);
		// Create base dir
		char* basedir = cbuild_path_base(out);
		if (!cbuild_dir_check(basedir)) if (!cbuild_dir_create(basedir)) return false;
		// Run command
		cbuild_log(CBUILD_LOG_TRACE,
			"Building source file '%s' into '%s'.", args.path, out);
		if (!cbuild_cmd_run(&cmd, .procs = &procs)) return false;
		// Cleanup
		cbuild_cmd_clear(&cmd);
		cbuild_temp_reset(0);
		return true;
	}
	if (!cbuild_sv_suffix(path, cbuild_sv_from_lit(".md"))) return true;
	cbuild_cmd_t cmd = {0};
	wikimk_cmd_append_pandoc_base_and_edit(&cmd, args.path);
	cbuild_cmd_append_many(&cmd, "--template", WIKIMK_TEMPLATE"/template.html");
	if (!cbuild_sv_contains_sv(path, cbuild_sv_from_lit(CODE_DOC_OUT)) &&
		!cbuild_sv_prefix(path, cbuild_sv_from_lit(CHANGELOG_DST))) {
		cbuild_cmd_append_many(&cmd, "-M", "valid-edit");
	}
	if (cbuild_sv_cmp(path, cbuild_sv_from_lit(CODE_DOC_SYMBOLS)) == 0) {
		cbuild_cmd_append_many(&cmd, "-M", "symbols-page");
	}
	// Input file
	cbuild_cmd_append(&cmd, args.path);
	// Output file
	path.size -= 3; // Chop '.md'
	cbuild_sv_chop_prefix(&path, cbuild_sv_from_lit(WIKI_SRC"/"));
	char* out = cbuild_temp_sprintf(WIKI_OUT"/"CBuildSVFmt".html", CBuildSVArg(path));
	cbuild_cmd_append_many(&cmd, "-o", out);
	// Create base dir
	char* basedir = cbuild_path_base(out);
	if (!cbuild_dir_check(basedir)) if (!cbuild_dir_create(basedir)) return false;
	// Run command
	cbuild_log(CBUILD_LOG_TRACE,
		"Building source file '%s' into '%s'.", args.path, out);
	if (!cbuild_cmd_run(&cmd, .procs = &procs)) return false;
	// Cleanup
	cbuild_cmd_clear(&cmd);
	cbuild_temp_reset(0);
	return true;
}
// TODO: This may be handled by template too, but this is rather complex code and I am too lazy to make this work.
bool wikimk_generate_nav_html_rec(cbuild_sb_t* out, const char* dir, const char* fmt_dir) {
	cbuild_pathlist_t files = {0};
	if (!cbuild_dir_list(dir, &files)) return false;
	cbuild_da_foreach(files, file) {
		const char* path = cbuild_temp_sprintf("%s/%s", dir, *file);
		cbuild_sv_t file_sv = cbuild_sv_from_cstr(*file);
		switch (cbuild_path_filetype(path)) {
		case CBUILD_FTYPE_REGULAR: {
			if (cbuild_sv_suffix(file_sv, cbuild_sv_from_lit(".url"))) {
				cbuild_sb_t file_content_sb = {0};
				if (!cbuild_file_read(path, &file_content_sb)) return false;
				cbuild_sv_t file_content = cbuild_sv_from_sb(file_content_sb);
				cbuild_sv_t name = cbuild_sv_chop_by_delim(&file_content, '\n');
				cbuild_sv_trim(&name);
				cbuild_sv_t url = cbuild_sv_chop_by_delim(&file_content, '\n');
				cbuild_sv_trim(&url);
				cbuild_sb_appendf(out, 
					"<li><a href=\""CBuildSVFmt"\">"CBuildSVFmt"</a></li>\n",
					CBuildSVArg(url), CBuildSVArg(name));
				cbuild_sb_clear(&file_content_sb);
				continue;
			}
			// TODO: Extract this somehow. Probably as a set of ignored files
			if (cbuild_sv_suffix(file_sv, cbuild_sv_from_lit("changelog.example.md"))) continue;
			// TODO: Extract this somehow. Probably as a set of added files
			if (!cbuild_sv_suffix(file_sv, cbuild_sv_from_lit("changelog.txt")) &&
					!cbuild_sv_suffix(file_sv, cbuild_sv_from_lit(".md"))) {
				continue;
			}
			cbuild_sv_chop_right_by_delim(&file_sv, '.'); // Chop file extension
			cbuild_sb_t file_content_sb = {0};
			if (!cbuild_file_read(path, &file_content_sb)) return false;
			cbuild_sv_t file_content = cbuild_sv_from_sb(file_content_sb);
			if (!cbuild_sv_chop_prefix(&file_content, cbuild_sv_from_lit("---\n"))) {
				cbuild_log_error("Invalid markdown yaml metadata in file %s.", path);
				return false;
			}
			cbuild_sv_t line = cbuild_sv_chop_by_delim(&file_content, '\n');
			bool found = false;
			while (cbuild_sv_cmp(line, cbuild_sv_from_lit("---")) != 0 && line.size != 0) {
				if (cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("title:"))) {
					cbuild_sv_trim(&line);
					// After 'title:' is chopped and lines is trimmed it now contains just title.
					cbuild_sb_appendf(out, 
						"<li><a href=\""SITE_BASE_URL"%s/"CBuildSVFmt".html\">"CBuildSVFmt"</a></li>\n",
						fmt_dir, CBuildSVArg(file_sv), CBuildSVArg(line));
					found = true;
					break;
				}
				line = cbuild_sv_chop_by_delim(&file_content, '\n');
			}
			if (!found) {
				cbuild_log_error("Malformed markdown file - it does not contain 'title' metadata.");
				return false;
			}
			cbuild_sb_clear(&file_content_sb);
		} break;
		case CBUILD_FTYPE_DIRECTORY: {
			const char* name_file_path = cbuild_temp_sprintf("%s/.dirname", path);
			if (cbuild_file_check(name_file_path)) {
				cbuild_sb_t dirname_sb = {0};
				if (!cbuild_file_read(name_file_path, &dirname_sb)) return false;
				cbuild_sv_t dirname = cbuild_sv_from_sb(dirname_sb);
				cbuild_sv_trim(&dirname);
				cbuild_sb_appendf(out,
					"<li><details open><summary>"CBuildSVFmt"</summary><ul>\n",
					CBuildSVArg(dirname));
				cbuild_sb_clear(&dirname_sb);
			} else {
				cbuild_sb_appendf(out,
					"<li><details open><summary>%s</summary><ul>\n", *file);
			}
			const char* new_fmt_dir = cbuild_temp_sprintf("%s/%s", fmt_dir, *file);
			if (!wikimk_generate_nav_html_rec(out, path, new_fmt_dir)) return false;
			cbuild_sb_appendf(out, "</ul></details></li>\n");
		} break;
		case CBUILD_FTYPE_MISSING:
		case CBUILD_FTYPE_SYMLINK:
		case CBUILD_FTYPE_OTHER:
		default:
			CBUILD_UNREACHABLE("Invalid filetype.");
			break;
		}
	}
	cbuild_pathlist_clear(&files);
	return true;
}
bool wikimk_generate_nav_html(const char* output, const char* src) {
	cbuild_sb_t out = {0};
	cbuild_log_trace("Generating navigation tree from '%s' into '%s'.", src, output);
	cbuild_sb_appendf(&out, "<ul>\n");
	size_t checkpoint = cbuild_temp_checkpoint(); // 8M should be enough for a few paths
	if (!wikimk_generate_nav_html_rec(&out, src, "")) return false;
	cbuild_temp_reset(checkpoint);
	cbuild_sb_appendf(&out, "</ul>\n");
	if (!cbuild_file_write(output, &out)) return false;
	cbuild_sb_clear(&out);
	return true;
}
// ----------------------------------------
// DocGen
// ----------------------------------------
// Helpers
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
		cbuild_sv_trim_left(&src);
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
	CBUILD_UNUSED(pname);
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
					void (*template_arg_or_field)(cbuild_sb_t* dst, argument_t arg) = NULL;
					if (cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("line:"))) {
						cbuild_sv_t tag = cbuild_sv_chop_by_delim(&line, ']');
						cbuild_da_foreach(*comments, cm) {
							if (cm->type == COMMENT_ANCHOR && 
								cbuild_sv_cmp(tag, cm->lines.data[0]) == 0) {
								TEMPLATE.look_in_source(dst, src, cm->first_line);
							}
						}
					} else if (
						(cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("pl:")) &&
							(template_arg_or_field = TEMPLATE.parsed_arg_in_list)) ||
						(cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("fl:")) &&
							(template_arg_or_field = TEMPLATE.parsed_field_in_list))) {
						char sep = '\0';
						cbuild_sv_t arg = cbuild_sv_chop_by_func(&line, md_dsl_arg_delim, &sep);
						if (sep == ':') {
							cbuild_sv_t type = cbuild_sv_chop_by_delim(&line, ']');
							argument_t indoc_arg = {
								.type = type,
								.name = arg,
							};
							template_arg_or_field(dst, indoc_arg);
						} else if (sep == ']') {
							bool found = false;
							if (pargs) {
								cbuild_da_foreach(*pargs, a) {
									if (cbuild_sv_cmp(arg, a->name) == 0) {
										found = true;
										if (template_arg_or_field == TEMPLATE.parsed_field_in_list && a->type.data == NULL) {
											TEMPLATE.parsed_enum_value_in_list(dst, *a);
										} else {
											template_arg_or_field(dst, *a);
										}
										break;
									}
								}
							}
							if (!found) {
								cbuild_log_warn(
									"No name '"CBuildSVFmt"' in symbol '"CBuildSVFmt"'.",
									CBuildSVArg(arg), CBuildSVArg(*pname));
								argument_t no_arg = {
									.type = cbuild_sv_from_lit("void"),
									.name = cbuild_sv_from_lit("no-name"),
								};
								template_arg_or_field(dst, no_arg);
							}
						} else {
							CBUILD_UNREACHABLE("Invalid markdown DSL separator.");
						}
					} else if (cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("r:"))) {
						// This is technically out of bound, but we chopped two chars at least,
						// so this access is valid.
						if (*line.data == ']') {
							cbuild_sv_chop(&line, 1);
							TEMPLATE.parsed_ret(dst, *pret_or_ptypename);
						} else {
							cbuild_sv_t text = cbuild_sv_chop_by_delim(&line, ']');
							TEMPLATE.parsed_ret(dst, text);
						}
					} else if ((cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("p:")) &&
							(template_arg_or_field = TEMPLATE.parsed_arg_standalone)) ||
						(cbuild_sv_chop_prefix(&line, cbuild_sv_from_lit("f:")) &&
							(template_arg_or_field = TEMPLATE.parsed_field_standalone))) {
						char sep = '\0';
						cbuild_sv_t arg = cbuild_sv_chop_by_func(&line, md_dsl_arg_delim, &sep);
						if (sep == ':') {
							cbuild_sv_t type = cbuild_sv_chop_by_delim(&line, ']');
							argument_t indoc_arg = {
								.type = type,
								.name = arg,
							};
							template_arg_or_field(dst, indoc_arg);
						} else if (sep == ']') {
							bool found = false;
							if (pargs) {
								cbuild_da_foreach(*pargs, a) {
									if (cbuild_sv_cmp(arg, a->name) == 0) {
										found = true;
										if (template_arg_or_field == TEMPLATE.parsed_field_in_list && a->type.data == NULL) {
											TEMPLATE.parsed_enum_value_in_list(dst, *a);
										} else {
											template_arg_or_field(dst, *a);
										}
										break;
									}
								}
							}
							if (!found) {
								cbuild_log_warn(
									"No name '"CBuildSVFmt"' in symbol '"CBuildSVFmt"'.",
									CBuildSVArg(arg), CBuildSVArg(*pname));
								argument_t no_arg = {
									.type = cbuild_sv_from_lit("void"),
									.name = cbuild_sv_from_lit("no-name"),
								};
								template_arg_or_field(dst, no_arg);
							}
						} else {
							CBUILD_UNREACHABLE("Invalid markdown DSL separator.");
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
	} break;
	case DECL_VARIABLE: {
		if (!PARSER.var_decl(lines, comment->last_line,
				&ret_or_typename, &name, &decl)) {
			cbuild_log_error("Could not parse variable declaration at line %zu.",
				comment->last_line);
			return false;
		}
	} break;
	case DECL_DEFINE: {
		if (!PARSER.define_decl(lines, comment->last_line,
				&name, &decl)) {
			cbuild_log_error("Could not parse macro declaration at line %zu.",
				comment->last_line);
			return false;
		}
	} break;
	case DECL_STRUCT: {
		if (!PARSER.struct_decl(lines, comment->last_line,
				&name, &args, &decl)) {
			cbuild_log_error("Could not parse structure declaration at line %zu.",
				comment->last_line);
			return false;
		}
	} break;
	case DECL_ENUM: {
		if (!PARSER.enum_decl(lines, comment->last_line,
				&name, &args, &decl)) {
			cbuild_log_error("Could not parse enumeration declaration at line %zu.",
				comment->last_line);
			return false;
		}
	} break;
	case DECL_TYPEDEF: {
		if (!PARSER.typedef_decl(lines, comment->last_line,
				&ret_or_typename, &name, &decl)) {
			cbuild_log_error("Could not parse typename declaration at line %zu.",
				comment->last_line);
			return false;
		}
	} break;
	case DECL_TYPEDEF_STRUCT: {
		if (!PARSER.typedef_struct_decl(lines, comment->last_line,
				&ret_or_typename, &name, &args, &decl)) {
			cbuild_log_error("Could not parse structure typedef declaration at line %zu.",
				comment->last_line);
			return false;
		}
	} break;
	case DECL_TYPEDEF_ENUM: {
		if (!PARSER.typedef_enum_decl(lines, comment->last_line,
				&ret_or_typename, &name, &args, &decl)) {
			cbuild_log_error("Could not parse enumeration typedef declaration at line %zu.",
				comment->last_line);
			return false;
		}
	} break;
	case DECL_TYPEDEF_FUNC: {
		if (!PARSER.typedef_func_decl(lines, comment->last_line,
				&ret_or_typename, &name, &args, &decl)) {
			cbuild_log_error("Could not parse structure typedef declaration at line %zu.",
				comment->last_line);
			return false;
		}
	} break;
	case DECL_NONE:
		CBUILD_UNREACHABLE("Invalid declaration type.");
		break;
	}
	if (!push_symbol(symbol_table, name, src)) return true;
	switch (decltype) {
	case DECL_FUNCTION:
		TEMPLATE.func_decl_start(dst);
		break;
	case DECL_VARIABLE:
		TEMPLATE.var_decl_start(dst);
		break;
	case DECL_DEFINE:
		TEMPLATE.define_decl_start(dst);
		break;
	case DECL_STRUCT:
		TEMPLATE.struct_decl_start(dst);
		break;
	case DECL_ENUM:
		TEMPLATE.enum_decl_start(dst);
		break;
	case DECL_TYPEDEF:
		TEMPLATE.typedef_decl_start(dst);
		break;
	case DECL_TYPEDEF_STRUCT:
		TEMPLATE.typedef_struct_decl_start(dst);
		break;
	case DECL_TYPEDEF_ENUM:
		TEMPLATE.typedef_enum_decl_start(dst);
		break;
	case DECL_TYPEDEF_FUNC:
		TEMPLATE.typedef_func_decl_start(dst);
		break;
	case DECL_NONE:
		CBUILD_UNREACHABLE("Invalid declaration type.");
		break;
	}
	TEMPLATE.decl_name(dst, name, src, comment->last_line + 1);
	TEMPLATE.decl_code(dst, decl);
	if (!docgen_comment_to_md(comment, dst, src, comments, "##", &ret_or_typename, &name, &args)) {
		return false;
	}
	#ifdef DEBUG_PARSER
		cbuild_sb_appendf(dst, "Ret/Typename: "CBuildSVFmt"\n", CBuildSVArg(ret_or_typename));
		cbuild_sb_appendf(dst, "Name: "CBuildSVFmt"\n", CBuildSVArg(name));
		cbuild_sb_append_cstr(dst, "Arguments:\n");
		cbuild_da_foreach(args, arg) {
			cbuild_sb_appendf(dst, "  - "CBuildSVFmt" of "CBuildSVFmt"\n",
				CBuildSVArg(arg->name), CBuildSVArg(arg->type));
		}
	#endif // DEBUG_PARSER
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
	cbuild_temp_reset(0);
	comments_t comments = {0};
	lines_t lines = {0};
	cbuild_sb_t dst = {0};
	void* file_content = NULL;
	if (!docgen_parse_file(src, &file_content, &comments, &lines)) return false;
	cbuild_sb_appendf(&dst, "---\n");
	cbuild_sb_appendf(&dst, "title: %s\n", cbuild_path_name(src));
	cbuild_sb_appendf(&dst, "---\n");
	cbuild_sb_appendf(&dst, "# %s\n\n", cbuild_path_name(src));
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
	if(!docgen_process_file(args.path, dst.data, args.context)) return false;
	cbuild_sb_clear(&dst);
	return true;
}
bool docgen_generate_symbols(const char* output, symbols_t* symbols) {
	cbuild_log_trace("Generating symbols list into '%s'", output);
	cbuild_sb_t dst = {0};
	cbuild_sb_append_cstr(&dst, "---\ntitle: All symbols exported by CBuild\n---\n");
	cbuild_sb_append_cstr(&dst, "# Symbols\n\n");
	cbuild_da_foreach(*symbols, symbol) {
		cbuild_temp_reset(0);
		TEMPLATE.symtab_entry(&dst, symbol->symbol_name, symbol->filename);
	}
	if (!cbuild_file_write(output, &dst)) return false;
	cbuild_sb_clear(&dst);
	return true;
}
// ----------------------------------------
// HTTP server
// ----------------------------------------
#define HTTP_SERVER_RECV_BUFFER_SIZE 4096
static const char* http_response_text[600] = {
	[200] = "OK",
	[404] = "Not found",
	[405] = "Method Not Allowed",
};
const char* http_mimedb(const char* file) {
	cbuild_sv_t path = cbuild_sv_from_cstr(file);
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".html"))) {
		return "text/html; charset=utf-8";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".htm"))) {
		return "text/html; charset=utf-8";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".css"))) {
		return "text/css";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".js"))) {
		return "application/javascript";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".json"))) {
		return "application/json";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".png"))) {
		return "image/png";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".jpg"))) {
		return "image/jpeg";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".jpeg"))) {
		return "image/jpeg";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".gif"))) {
		return "image/gif";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".svg"))) {
		return "image/svg+xml";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".ico"))) {
		return "image/x-icon";
	} else if (cbuild_sv_suffix(path, cbuild_sv_from_lit(".txt"))) {
		return "text/plain; charset=utf-8";
	} else {
		return "application/octet-stream";
	}
}
void http_serve_file(int socket, const char* path, int response) {
	cbuild_sb_t file = {0};
	if (!cbuild_file_read(path, &file)) {
		cbuild_log_error("Could not read file '%s'.", path);
		return;
	}
	const char* headers = cbuild_temp_sprintf(
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %zu\r\n"
		"Connection: close\r\n"
		"\r\n",
		response, http_response_text[response],
		http_mimedb(path),
		file.size);
	send(socket, headers, strlen(headers), 0);
	send(socket, file.data, file.size, 0);
	cbuild_sb_clear(&file);
}
bool http_serve_dir(const char* dir, uint16_t port) {
	// Create socket
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0) {
		cbuild_log_error("Failed to create socket, error: \"%s\".",
			strerror(errno));
		return false;
	}
	int opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, 
		&opt, sizeof(opt));
	// Bind socket
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	if(bind(server_socket,
			(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		cbuild_log_error("Failed to bind socket to port %d, error: '%s'.",
			port, strerror(errno));
		return false;
	}
	// Listen for a client
	if(listen(server_socket, 10) < 0) {
		cbuild_log_error("Failed to listen for a client, error: '%s'.",
			strerror(errno));
		return false;
	}
	// Main loop of a server
	while(true) {
		cbuild_temp_reset(0);
		// Accept client
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int client_socket = accept(server_socket,
			(struct sockaddr *)&client_addr, &client_addr_len);
		if(client_socket < 0) {
			cbuild_log_error("Failed to accept client, error: \"%s\".",
				strerror(errno));
		}
		// Read request
		char buf[HTTP_SERVER_RECV_BUFFER_SIZE];
		// TODO: Dynamic buffer
		ssize_t len = recv(client_socket, buf, sizeof(buf), 0);
		if(len < 0) {
			cbuild_log_error("Received malformed request from client.");
			goto defer1;
		}
		cbuild_sv_t request = cbuild_sv_from_parts(buf, (size_t)len);
		cbuild_sv_t type = cbuild_sv_chop_by_delim(&request, ' ');
		cbuild_sv_t path = cbuild_sv_chop_by_delim(&request, ' ');
		cbuild_sv_t protocol =
			cbuild_sv_chop_by_sv(&request, cbuild_sv_from_cstr("\r\n"));
		if(cbuild_sv_cmp(type, cbuild_sv_from_cstr("GET")) == 0 &&
			cbuild_sv_cmp(protocol, cbuild_sv_from_cstr("HTTP/1.1")) == 0) {
			cbuild_log_trace("Received request from client.");
			cbuild_log_trace("Requested path \""CBuildSVFmt"\".",
				CBuildSVArg(path));
			cbuild_sb_t fpath = {0};
			cbuild_sb_append_cstr(&fpath, dir);
			// TODO: Expands '%' in path
			if(cbuild_sv_suffix(path, cbuild_sv_from_cstr("/"))) {
				cbuild_sb_append_sv(&fpath, path);
				cbuild_sb_append_cstr(&fpath, "index.html");
			} else {
				cbuild_sb_append_sv(&fpath, path);
			}
			cbuild_sb_append_null(&fpath);
			if(cbuild_sv_contains_sv(path, cbuild_sv_from_cstr(".."))) {
				cbuild_log_warn("Path contains \"..\", aborting.");
			} else {
				cbuild_log_trace("Path resolves to \"%s\"", fpath.data);
				if(!cbuild_file_check(fpath.data)) {
					cbuild_log_warn("File not found, sending 404.");
					const char* epath = cbuild_temp_sprintf("%s/404.html", dir);
					http_serve_file(client_socket, epath, 404);
				} else {
					cbuild_log_trace("File found, sending 200.");
					http_serve_file(client_socket, fpath.data, 200);
				}
			}
			cbuild_sb_clear(&fpath);
		} else {
			cbuild_log_warn("Received invalid request from client.");
			if((cbuild_sv_cmp(type, cbuild_sv_from_cstr("CONNECT")) == 0 ||
					cbuild_sv_cmp(type, cbuild_sv_from_cstr("DELETE")) == 0 ||
					cbuild_sv_cmp(type, cbuild_sv_from_cstr("HEAD")) == 0 ||
					cbuild_sv_cmp(type, cbuild_sv_from_cstr("OPTIONS")) == 0 ||
					cbuild_sv_cmp(type, cbuild_sv_from_cstr("PATCH")) == 0 ||
					cbuild_sv_cmp(type, cbuild_sv_from_cstr("POST")) == 0 ||
					cbuild_sv_cmp(type, cbuild_sv_from_cstr("PUT")) == 0 ||
					cbuild_sv_cmp(type, cbuild_sv_from_cstr("TRACE")) == 0) &&
				cbuild_sv_cmp(protocol, cbuild_sv_from_cstr("HTTP/1.1")) == 0) {
				cbuild_log_warn("Invalid request type, sending 405.");
				const char* epath = cbuild_temp_sprintf("%s/405.html", dir);
				http_serve_file(client_socket, epath, 405);
			}
		}
	defer1:
		close(client_socket);
		cbuild_log_trace("Finished handling request from client.");
	}
	close(server_socket);
	return true;
}
