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
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "cbuild.h"
#include "sys/socket.h"
#include "netinet/in.h"
#define CBUILD_IMPLEMENTATION
#include "cbuild.h"
// Config
#define WIKIMK_WIKI_NAME "My cool wiki"
#define WIKIMK_WIKI_AUTHOR "WolodiaM"
#define WIKIMK_WIKI_LICENSE "GFDL-1.3-or-later"
#define WIKIMK_WIKI_EDIT_BASE "https://github.com/WolodiaM/CBuild/edit/master/wiki/wikimk/src"
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
	if(!cbuild_cmd_run(&cmd)) {
		return false;
	}
	// Clean-up
	cbuild_sb_clear(&src);
	cbuild_sb_clear(&dst);
	return true;
}
bool gen_index_redirect(cbuild_sb_t index_cfg) {
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
	if(!cbuild_cmd_run(&cmd)) {
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
	if(!cbuild_cmd_run(&cmd, .fdstdout = &wr)) {
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
	cbuild_da_foreach(list, file){
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
					"<li><a href=\""CBuildSVFmt".html\">",
					CBuildSVArg(filename_sv));
				gentoc_get_title(filepath.data, out);
				cbuild_sb_appendf(out, "</a></li>\n");
			} else if(cbuild_sv_suffix(filename_sv, cbuild_sv_from_cstr(".url"))) {
				cbuild_sb_t buffer = {0};
				cbuild_file_read(filepath.data, &buffer);
				cbuild_sv_t buffer_sv = cbuild_sb_to_sv(buffer);
				cbuild_sv_t	name_sv = cbuild_sv_chop_by_delim(&buffer_sv, '\n');
				cbuild_sv_t url = cbuild_sv_chop_by_delim(&buffer_sv, '\n');
				cbuild_sb_appendf(out, "<li><a href =\"");
				if(cbuild_sv_prefix(url, cbuild_sv_from_cstr("http"))) {
					cbuild_sb_append_sv(out, url);
				} else {
					size_t checkpoint = cbuild_temp_checkpoint();
					char *base = cbuild_path_base(filename.data);
					cbuild_sb_appendf(out, "%s"CBuildSVFmt, base, CBuildSVArg(url));
					cbuild_temp_reset(checkpoint);
				}
				cbuild_sb_appendf(out, "\">"CBuildSVFmt"</a></li>\n", CBuildSVArg(name_sv));
			}
		} break;
		case CBUILD_FTYPE_DIRECTORY: {
			cbuild_sb_t dirname_path = {0};
			cbuild_sb_appendf(&dirname_path, "%s/.dirname", filename.data);
			cbuild_sb_append_null(&dirname_path);
			if(cbuild_file_check(dirname_path.data)) {
				cbuild_sb_t dirname = {0};
				cbuild_file_read(dirname_path.data, &dirname);
				cbuild_sv_t	dirname_sv_full = cbuild_sb_to_sv(dirname);
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
	cbuild_da_foreach(list, file){
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
	cbuild_da_foreach(root_list, root_file){
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
					"<li><a href=\""CBuildSVFmt".html\">",
					CBuildSVArg(filename));
				gentoc_get_title(root_filepath.data, &nav_html);
				cbuild_sb_appendf(&nav_html, "</a></li>\n");
			} else if(cbuild_sv_suffix(filename, cbuild_sv_from_cstr(".url"))) {
				cbuild_sb_t buffer = {0};
				cbuild_file_read(root_filepath.data, &buffer);
				cbuild_sv_t buffer_sv = cbuild_sb_to_sv(buffer);
				cbuild_sv_t	name = cbuild_sv_chop_by_delim(&buffer_sv, '\n');
				cbuild_sv_t url = cbuild_sv_chop_by_delim(&buffer_sv, '\n');
				cbuild_sb_appendf(&nav_html, "<li><a href =\"");
				if(cbuild_sv_prefix(url, cbuild_sv_from_cstr("http"))) {
					cbuild_sb_append_sv(&nav_html, url);
				} else {
					cbuild_sb_appendf(&nav_html, ""CBuildSVFmt, CBuildSVArg(url));
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
				cbuild_sv_t	dirname_sv_full = cbuild_sb_to_sv(dirname);
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
	cbuild_da_foreach(root_list, root_file){
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
				if(!gen_index_redirect(index_cfg)){
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
// --------------------------------------------
// Server
// --------------------------------------------
#define HTTP_SERVER_RECV_BUFFER_SIZE 4096
cbuild_map_t http_server_mimecache = {0};
#define http_server_write_mimecache(extension, mime)                           \
	key = extension;                                                             \
	elem = cbuild_map_get_or_alloc(&http_server_mimecache, key);                 \
	elem[0] = key; elem[1] = mime;
void http_server_build_mimecache(void) {
	http_server_mimecache.key_size = 0;
	http_server_mimecache.elem_size = sizeof(char*) * 2;
	cbuild_map_init(&http_server_mimecache, 64);
	char** elem;
	char* key;
	http_server_write_mimecache("html", "text/html; charset=utf-8");
	http_server_write_mimecache("htm",  "text/html; charset=utf-8");
	http_server_write_mimecache("css",  "text/css");
	http_server_write_mimecache("js",   "application/javascript");
	http_server_write_mimecache("json", "application/json");
	http_server_write_mimecache("png",  "image/png");
	http_server_write_mimecache("jpg",  "image/jpeg");
	http_server_write_mimecache("jpeg", "image/jpeg");
	http_server_write_mimecache("gif",  "image/gif");
	http_server_write_mimecache("svg",  "image/svg+xml");
	http_server_write_mimecache("ico",  "image/x-icon");
	http_server_write_mimecache("txt",  "text/plain; charset=utf-8");
	// TODO: More mimetypes ?
}
char*	http_server_mime(const char* filepath) {
	size_t checkpoint = cbuild_temp_checkpoint();
	char* ext = cbuild_path_ext(filepath);
	char** elem = cbuild_map_get(&http_server_mimecache, ext);
	cbuild_temp_reset(checkpoint);
	if(elem == NULL) {
		return "application/octet-stream";
	} else {
		return elem[1];
	}
}
// False if file should not be sent
bool http_server_serve_headers(int client_socket, int responce, const char* mime, cbuild_sb_t file) {
	char* header;
	size_t header_len;
	static const char* responce_text[600] = {
		[200] = "OK",
		[400] = "Bad Request",
		[404] = "Not found",
		[405] = "Method Not Allowed",
	};
	ssize_t hlen = asprintf(&header,
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %zu\r\n"
		"Connection: close\r\n"
		"\r\n",
		responce, responce_text[responce],
		mime,
		file.size);
	if(hlen > 0) {
		header_len = (size_t)hlen;
		send(client_socket, header, (size_t)header_len, 0);
	}
	free(header);
	return true;
}
// static_ means that responce html filename should be generated based on on responce code. filepath is root then.
// TODO: 304 responces
void http_server_serve_file(int client_socket, const char* filepath, int responce, bool static_) {
	cbuild_sb_t file = {0};
	char* mime;
	if(static_ == false) {
		cbuild_file_read(filepath, &file);
		mime = http_server_mime(filepath);
	} else {
		cbuild_sb_t fpath = {0};
		cbuild_sb_appendf(&fpath, "%s/%d.html", filepath, responce);
		cbuild_sb_append_null(&fpath);
		cbuild_file_read(fpath.data, &file);
		mime = http_server_mime(fpath.data);
		cbuild_sb_clear(&fpath);
	}
	if(http_server_serve_headers(client_socket, responce, mime, file)) {
		send(client_socket, file.data, file.size, 0);
	}
	cbuild_sb_clear(&file);
}
bool http_server(short unsigned int port, const char* root) {
	// Init mimecache
	http_server_build_mimecache();
	// Create socket
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0) {
		cbuild_log_error("Failed to create socket, error: \"%s\".",	strerror(errno));
		return false;
	}
	int opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	// Bind socket
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	if(bind(server_socket,
			(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		cbuild_log_error("Failed to bind socket to port %d, error: \"%s\".",
			port, strerror(errno));
		return false;
	}
	// Listen for a client
	if(listen(server_socket, 10) < 0) {
		cbuild_log_error("Failed to listen for a client, error: \"%s\".",
			strerror(errno));
		return false;
	}
	// Main loop of a server
	while(true) {
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
				CBuildSBArg(path));
			cbuild_sb_t fpath = {0};
			cbuild_sb_append_cstr(&fpath, root);
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
					http_server_serve_file(client_socket, root, 404, true);
				} else {
					cbuild_log_trace("File found, sending 200.");
					http_server_serve_file(client_socket, fpath.data, 200, false);
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
				http_server_serve_file(client_socket, root, 405, true);
			}
		}
	defer1:
		close(client_socket);
		cbuild_log_trace("Finished handling request from client.");
	}
	close(server_socket);
	return true;
}
// --------------------------------------------
// Code
// --------------------------------------------
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
		if(!http_server(8000, WIKIMK_DIR_OUT)) return 1;
	}
	return 0;
}
