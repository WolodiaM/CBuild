/**
 * @file cbuild.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief WikiMK documentation wiki build system
 * Uses pandoc as its base and templating engine
 *
 * @date 2025-07-16
 * @copyright (C) 2025 WolodiaM
 * @license GPL-3.0-or-later
 *
 * Copyright (C) 2025 WolodiaM
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
// Includes
#define CBUILD_IMPLEMENTATION
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "cbuild.h"
// Config

#define WIKIMK_WIKI_NAME "My cool wiki"
#define WIKIMK_WIKI_AUTHOR "WolodiaM"
#define WIKIMK_WIKI_LICENSE "GFDL-1.3-or-later"
#define WIKIMK_WIKI_EDIT_BASE "https://gitlab.com/cbuild/cbuild/edit/master/wiki/wikimk/src"

#define WIKIMK_DIR_SRC "wiki/wikimk/src"
#define WIKIMK_DIR_TEMPLATE "wiki/wikimk/template"
#define WIKIMK_DIR_SCRIPT "wiki/wikimk/script"
#define WIKIMK_DIR_STYLE "wiki/wikimk/style"
#define WIKIMK_DIR_OUT "wiki/out"
// Code
bool build_md_to_html(cbuild_sv_t filename) {
	// Init
	cbuild_cmd_t cmd = {0};
	cbuild_cmd_append(&cmd, "pandoc");
	// Args
	cbuild_cmd_append_many(&cmd, "--template", WIKIMK_DIR_TEMPLATE "/template.html");
	cbuild_cmd_append_many(&cmd,
	  "-M", "name:" WIKIMK_WIKI_NAME,
	  "-M", "author:" WIKIMK_WIKI_AUTHOR,
	  "-M", "license:" WIKIMK_WIKI_LICENSE);
	// Files
	cbuild_sb_t src = {0};
	cbuild_sb_appendf(&src,
	  WIKIMK_DIR_SRC "/" CBuildSVFmt ".md", CBuildSVArg(filename));
	cbuild_sb_append_null(&src);
	cbuild_cmd_append(&cmd, src.data);
	cbuild_sb_t dst = {0};
	cbuild_sb_appendf(&dst,
	  WIKIMK_DIR_OUT "/" CBuildSVFmt ".html", CBuildSVArg(filename));
	cbuild_sb_append_null(&dst);
	// Edit block
	cbuild_sb_t edit_url = {0};
	cbuild_sb_appendf(&edit_url,
	  "EDIT-URL:"  WIKIMK_WIKI_EDIT_BASE "/" CBuildSVFmt ".md", CBuildSVArg(filename));
	cbuild_sb_append_null(&edit_url);
	cbuild_cmd_append_many(&cmd, "-M", edit_url.data);
	cbuild_sb_t edit_filename = {0};
	cbuild_sb_appendf(&edit_filename,
	  "EDIT-FILENAME:/" CBuildSVFmt ".md", CBuildSVArg(filename));
	cbuild_sb_append_null(&edit_filename);
	cbuild_cmd_append_many(&cmd, "-M", edit_filename.data);
	// Append files to cmd
	cbuild_cmd_append_many(&cmd, "-o", dst.data);
	// Call to pandoc
	cbuild_log(CBUILD_LOG_TRACE,
	  "Building source file %s into %s.", src.data, dst.data);
	if(!cbuild_cmd_sync(cmd)) {
		return false;
	}
	// Clean-up
	cbuild_sb_clear(&src);
	cbuild_sb_clear(&dst);
	return true;
}
bool gen_index_redirect(cbuild_sb_t* index_cfg) {
	cbuild_sv_t cfg =	cbuild_sb_to_sv(index_cfg);
	cbuild_sv_t path = cbuild_sv_chop_by_delim(&cfg, '\n');
	cbuild_sb_t redirect_meta = {0};
	cbuild_sb_appendf(&redirect_meta, "REDIRECT:"CBuildSVFmt, CBuildSVArg(path));
	cbuild_sb_append_null(&redirect_meta);
	cbuild_cmd_t cmd = {0};
	cbuild_cmd_append_many(&cmd, "pandoc", "-t", "html5",
	  "-M", redirect_meta.data,
	  "-M", "name:" WIKIMK_WIKI_NAME,
	  "-M", "author:" WIKIMK_WIKI_AUTHOR,
	  "-M", "license:" WIKIMK_WIKI_LICENSE,
		"-M", "title:", "-f", "markdown", "--quiet",
	  "--template", WIKIMK_DIR_TEMPLATE "/redirect.html",
	  "-o", WIKIMK_DIR_OUT "/index.html",
		"/dev/null");
	if(!cbuild_cmd_sync(cmd)) {
		return false;
	}
	return true;
}
bool gentoc_get_title(const char* path, cbuild_sb_t* out) {
	char buff[1025] = {0};
	cbuild_fd_t rd, wr;
	cbuild_fd_open_pipe(&rd, &wr);
	cbuild_cmd_t cmd = {0};
	cbuild_cmd_append_many(&cmd,
	  "pandoc",
	  "--template", WIKIMK_DIR_TEMPLATE "/metadata-extract.txt",
	  (char*)path);
	if(!cbuild_cmd_sync_redirect(cmd, (cbuild_cmd_fd_t) {
	.fdstdout = wr,
	.fdstdin = CBUILD_INVALID_FD,
	.fdstderr = CBUILD_INVALID_FD
})) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot extract metadata!");
		return false;
	}
	cbuild_fd_close(wr);
	if(read(rd, buff, 1024) < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot extract metadata!");
		return false;
	}
	cbuild_cmd_clear(&cmd);
	buff[strlen(buff) -1] = '\0';
	cbuild_sb_append_cstr(out, buff);
	return true;
}
bool gentoc_subdir_recursively(const char* name, cbuild_sb_t* out) {
	// Get listing
	cbuild_pathlist_t list = {0};
	cbuild_sb_t dir_path = {0};
	cbuild_sb_appendf(&dir_path, WIKIMK_DIR_SRC "/%s", name);
	cbuild_sb_append_null(&dir_path);
	if(!cbuild_dir_list(dir_path.data, &list)) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot list directory %s!", dir_path.data);
		return false;
	}
	cbuild_sb_clear(&dir_path);
	// Build
	cbuild_da_foreach(&list, file) {
		// Generate full filename
		cbuild_sb_t filename = {0};
		cbuild_sb_appendf(&filename, "%s/%s", name, *file);
		cbuild_sb_append_null(&filename);
		// Generate full path
		cbuild_sb_t filepath = {0};
		cbuild_sb_appendf(&filepath, WIKIMK_DIR_SRC "/%s", filename.data);
		cbuild_sb_append_null(&filepath);
		// Check type
		cbuild_filetype_t type = cbuild_path_filetype(filepath.data);
		switch(type) {
		case CBUILD_FTYPE_REGULAR: {
			cbuild_sv_t filename_sv = cbuild_sv_from_cstr(filename.data);
			if(cbuild_sv_suffix(filename_sv, cbuild_sv_from_cstr(".md"))) {
				filename_sv.size -= 3;
				cbuild_sb_appendf(out,
				  "<li><a href=\"/"CBuildSVFmt".html\">",
				  CBuildSVArg(filename_sv));
				gentoc_get_title(filepath.data, out);
				cbuild_sb_appendf(out, "</a></li>\n");
			} else if(cbuild_sv_suffix(filename_sv, cbuild_sv_from_cstr(".url"))) {
				cbuild_sb_t buffer = {0};
				cbuild_file_read(filepath.data, &buffer);
				cbuild_sv_t buffer_sv = cbuild_sb_to_sv(&buffer);
				cbuild_sv_t	name = cbuild_sv_chop_by_delim(&buffer_sv, '\n');
				cbuild_sv_t url = cbuild_sv_chop_by_delim(&buffer_sv, '\n');
				cbuild_sb_appendf(out, "<li><a href =\"");
				if(cbuild_sv_prefix(url, cbuild_sv_from_cstr("http"))) {
					cbuild_sb_append_sv(out, url);
				} else {
					const char *base = cbuild_path_base(filename.data);
					cbuild_sb_appendf(out, "/%s"CBuildSVFmt, base, CBuildSVArg(url));
					free((void*)base);
				}
				cbuild_sb_appendf(out, "\">"CBuildSVFmt"</a></li>\n", CBuildSVArg(name));
			}
		} break;
		case CBUILD_FTYPE_DIRECTORY: {
			cbuild_sb_t dirname_path = {0};
			cbuild_sb_appendf(&dirname_path, "%s/.dirname", filename.data);
			cbuild_sb_append_null(&dirname_path);
			if(cbuild_file_check(dirname_path.data)) {
				cbuild_sb_t dirname = {0};
				cbuild_file_read(dirname_path.data, &dirname);
				cbuild_sv_t	dirname_sv_full = cbuild_sb_to_sv(&dirname);
				cbuild_sv_t dirname_sv = cbuild_sv_chop_by_delim(&dirname_sv_full, '\n');
				cbuild_sb_appendf(out, "<li>"CBuildSVFmt"\n<ul>\n", CBuildSVArg(dirname_sv));
				cbuild_sb_clear(&dirname);
			} else {
				cbuild_sv_t fname = cbuild_sv_from_cstr(filename.data);
				cbuild_sv_chop_by_delim(&fname, '/');
				cbuild_sb_appendf(out, "<li>"CBuildSVFmt"\n<ul>\n", CBuildSVArg(fname));
			}
			cbuild_sb_clear(&dirname_path);
			if(!gentoc_subdir_recursively(filename.data, out)) {
				return false;
			}
			cbuild_sb_appendf(out, "</ul></li>\n");
		} break;
		default: CBUILD_UNREACHABLE("File type not supported!");
		}
		cbuild_sb_clear(&filepath);
		cbuild_sb_clear(&filename);
	}
	cbuild_pathlist_clear(&list);
	return true;
}
bool build_subdir_recursively(const char* name) {
	cbuild_log(CBUILD_LOG_TRACE, "Building directory %s.", name);
	// Get listing
	cbuild_pathlist_t list = {0};
	cbuild_sb_t dir_path = {0};
	cbuild_sb_appendf(&dir_path, WIKIMK_DIR_SRC "/%s", name);
	cbuild_sb_append_null(&dir_path);
	if(!cbuild_dir_list(dir_path.data, &list)) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot list directory %s!", dir_path.data);
		return false;
	}
	cbuild_sb_clear(&dir_path);
	// Create output directory
	cbuild_sb_t out_dir = {0};
	cbuild_sb_appendf(&out_dir, WIKIMK_DIR_OUT "/%s", name);
	cbuild_sb_append_null(&out_dir);
	if(!cbuild_dir_check(out_dir.data)) {
		if(!cbuild_dir_create(out_dir.data)) {
			return false;
		}
	}
	cbuild_sb_clear(&out_dir);
	// Build
	cbuild_da_foreach(&list, file) {
		// Generate full filename
		cbuild_sb_t filename = {0};
		cbuild_sb_appendf(&filename, "%s/%s", name, *file);
		cbuild_sb_append_null(&filename);
		// Generate full path
		cbuild_sb_t filepath = {0};
		cbuild_sb_appendf(&filepath, WIKIMK_DIR_SRC "/%s", filename.data);
		cbuild_sb_append_null(&filepath);
		// Check type
		cbuild_filetype_t type = cbuild_path_filetype(filepath.data);
		cbuild_sb_clear(&filepath);
		switch(type) {
		case CBUILD_FTYPE_REGULAR: {
			cbuild_sv_t filename_sv = cbuild_sv_from_cstr(filename.data);
			if(cbuild_sv_suffix(filename_sv, cbuild_sv_from_cstr(".md"))) {
				filename_sv.size -= 3;
				if(!build_md_to_html(filename_sv)) {
					cbuild_log(CBUILD_LOG_ERROR, "Cannot build %s!", filename.data);
					return false;
				}
			}
		} break;
		case CBUILD_FTYPE_DIRECTORY: {
			if(!build_subdir_recursively(filename.data)) {
				return false;
			}
		} break;
		default: CBUILD_UNREACHABLE("File type not supported!");
		}
		cbuild_sb_clear(&filename);
	}
	cbuild_pathlist_clear(&list);
	return true;
}
bool build() {
	// Get root listing
	cbuild_pathlist_t root_list = {0};
	if(!cbuild_dir_list(WIKIMK_DIR_SRC, &root_list)) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot list directory " WIKIMK_DIR_SRC "!");
		return false;
	}
	// Generate nav list
	cbuild_log(CBUILD_LOG_TRACE, "Generating navigation tree.");
	cbuild_sb_t nav_html = {0};
	cbuild_sb_appendf(&nav_html, "<ul>\n");
	cbuild_da_foreach(&root_list, root_file) {
		cbuild_sb_t root_filepath = {0};
		cbuild_sb_appendf(&root_filepath, WIKIMK_DIR_SRC "/%s", *root_file);
		cbuild_sb_append_null(&root_filepath);
		cbuild_filetype_t type = cbuild_path_filetype(root_filepath.data);
		switch(type) {
		case CBUILD_FTYPE_REGULAR: {
			cbuild_sv_t filename = cbuild_sv_from_cstr(*root_file);
			if(cbuild_sv_suffix(filename, cbuild_sv_from_cstr(".md"))) {
				filename.size -= 3;
				cbuild_sb_appendf(&nav_html,
				  "<li><a href=\"/"CBuildSVFmt".html\">",
				  CBuildSVArg(filename));
				gentoc_get_title(root_filepath.data, &nav_html);
				cbuild_sb_appendf(&nav_html, "</a></li>\n");
			} else if(cbuild_sv_suffix(filename, cbuild_sv_from_cstr(".url"))) {
				cbuild_sb_t buffer = {0};
				cbuild_file_read(root_filepath.data, &buffer);
				cbuild_sv_t buffer_sv = cbuild_sb_to_sv(&buffer);
				cbuild_sv_t	name = cbuild_sv_chop_by_delim(&buffer_sv, '\n');
				cbuild_sv_t url = cbuild_sv_chop_by_delim(&buffer_sv, '\n');
				cbuild_sb_appendf(&nav_html, "<li><a href =\"");
				if(cbuild_sv_prefix(url, cbuild_sv_from_cstr("http"))) {
					cbuild_sb_append_sv(&nav_html, url);
				} else {
					cbuild_sb_appendf(&nav_html, "/"CBuildSVFmt, CBuildSVArg(url));
				}
				cbuild_sb_appendf(&nav_html, "\">"CBuildSVFmt"</a></li>\n", CBuildSVArg(name));
			}
		} break;
		case CBUILD_FTYPE_DIRECTORY: {
			cbuild_sb_t dirname_path = {0};
			cbuild_sb_appendf(&dirname_path, "%s/.dirname", root_filepath.data);
			cbuild_sb_append_null(&dirname_path);
			if(cbuild_file_check(dirname_path.data)) {
				cbuild_sb_t dirname = {0};
				cbuild_file_read(dirname_path.data, &dirname);
				cbuild_sv_t	dirname_sv_full = cbuild_sb_to_sv(&dirname);
				cbuild_sv_t dirname_sv = cbuild_sv_chop_by_delim(&dirname_sv_full, '\n');
				cbuild_sb_appendf(&nav_html, "<li>"CBuildSVFmt"\n<ul>\n", CBuildSVArg(dirname_sv));
				cbuild_sb_clear(&dirname);
			} else {
				cbuild_sb_appendf(&nav_html, "<li>%s\n<ul>\n", *root_file);
			}
			cbuild_sb_clear(&dirname_path);
			if(!gentoc_subdir_recursively(*root_file, &nav_html)) {
				return false;
			}
			cbuild_sb_appendf(&nav_html, "</ul></li>\n");
		} break;
		default: CBUILD_UNREACHABLE("File type not supported!"); break;
		}
		cbuild_sb_clear(&root_filepath);
	}
	cbuild_sb_appendf(&nav_html, "</ul>\n");
	if(!cbuild_file_write(WIKIMK_DIR_TEMPLATE "/nav.html", &nav_html)) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot create navigation section!");
		return false;
	}
	cbuild_sb_clear(&nav_html);
	// Build
	cbuild_da_foreach(&root_list, root_file) {
		cbuild_sb_t root_filepath = {0};
		cbuild_sb_appendf(&root_filepath, WIKIMK_DIR_SRC "/%s", *root_file);
		cbuild_sb_append_null(&root_filepath);
		cbuild_filetype_t type = cbuild_path_filetype(root_filepath.data);
		switch(type) {
		case CBUILD_FTYPE_REGULAR: {
			cbuild_sv_t filename = cbuild_sv_from_cstr(*root_file);
			if(cbuild_sv_suffix(filename, cbuild_sv_from_cstr(".md"))) {
				filename.size -= 3;
				if(!build_md_to_html(filename)) {
					cbuild_log(CBUILD_LOG_ERROR, "Cannot build %s!", *root_file);
					return false;
				}
			} else if(cbuild_sv_cmp(filename, cbuild_sv_from_cstr("index.cfg")) == 0) {
				cbuild_sb_t index_cfg = {0};
				cbuild_file_read(root_filepath.data, &index_cfg);
				if(!gen_index_redirect(&index_cfg)) {
					return false;
				}
				cbuild_sb_clear(&index_cfg);
			}
		} break;
		case CBUILD_FTYPE_DIRECTORY: {
			if(!build_subdir_recursively(*root_file)) {
				return false;
			}
		} break;
		default: CBUILD_UNREACHABLE("File type not supported!"); break;
		}
		cbuild_sb_clear(&root_filepath);
	}
	cbuild_pathlist_clear(&root_list);
	// Copy needed files
	const char *dirs[] = {
		WIKIMK_DIR_SCRIPT,
		WIKIMK_DIR_OUT "/script",
		WIKIMK_DIR_STYLE,
		WIKIMK_DIR_OUT "/style"
	};
	for(size_t i = 0; i < cbuild_arr_len(dirs); i += 2) {
		if(cbuild_dir_check(dirs[i + 1])) {
			cbuild_dir_remove(dirs[i + 1]);
		}
		cbuild_log(CBUILD_LOG_TRACE, "Copying %s to %s.",
		  dirs[i], dirs[i + 1]);
		cbuild_dir_copy(dirs[i], dirs[i + 1]);
	}
	return true;
}
// Code
int main(int argc, char** argv) {
	cbuild_selfrebuild(argc, argv);
	cbuild_shift(argv, argc);
	if(!cbuild_dir_check(WIKIMK_DIR_SRC)) {
		cbuild_log(CBUILD_LOG_ERROR, "Source directory does not exist!");
		return 1;
	}
	if(!cbuild_dir_check(WIKIMK_DIR_OUT)) {
		if(!cbuild_dir_create(WIKIMK_DIR_OUT)) {
			cbuild_log(CBUILD_LOG_ERROR, "Cannot create output directory!");
			return 1;
		}
	}
	const char *subcmd = cbuild_shift_expect(argv, argc, "Subcommand is required!");
	if(strcmp(subcmd, "build") == 0) {
		if(!build()) {
			return 1;
		}
	} else if(strcmp(subcmd, "serve") == 0) {
		cbuild_log(CBUILD_LOG_INFO, "Serving site using python SimpleHTTPServer.");
		cbuild_cmd_t cmd = {0};
		cbuild_cmd_append_many(&cmd, "python", "-m", "http.server", "8000", "-d", WIKIMK_DIR_OUT);
		if(!cbuild_cmd_sync(cmd)) {
			return 1;
		}
	}
	return 0;
}
