#define CBUILD_IMPLEMENTATION
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "cbuild.h"
cbuild_proclist_t procs = {0};
cbuild_cmd_t cmd = {0};
cbuild_cmd_t link_cmd = {0};
bool walker(cbuild_dir_walk_func_args_t args) {
	cbuild_sv_t path = cbuild_sv_from_cstr(args.path);
	if (!cbuild_sv_suffix(path, cbuild_sv_from_lit(".c"))) return true;
	char* opath = cbuild_temp_sprintf("%s.o", args.path);
	for (size_t i = 0; i < strlen(opath) - 2; i++) if (opath[i] == '/') opath[i] = '.';
	int change = cbuild_compare_mtime(opath, args.path);
	cbuild_cmd_append(&link_cmd, opath);
	if (change == 0) return true;
	if (change < 0) return false;
	if (change > 0) {
		cbuild_cmd_append_many(&cmd, CC, "-c", args.path, "-o", opath);
		if (!cbuild_cmd_run(&cmd, .procs = &procs)) return false;
		cmd.size = 0;
	}
	return true;
}
int main(int argc, char** argv) {
	cbuild_selfrebuild(argc, argv);
	cbuild_cmd_append_many(&link_cmd, CC, "-o", "app.run");
	cbuild_dir_walk("src", walker);
	cbuild_procs_wait(procs);
	if (!cbuild_cmd_run(&link_cmd)) return 1;
	cbuild_proclist_clear(&procs);
	cbuild_cmd_clear(&cmd);
	cbuild_cmd_clear(&link_cmd);
	cbuild_temp_reset(0);
	return 0;
}
