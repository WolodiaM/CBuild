#define CBUILD_IMPLEMENTATION
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "cbuild.h"
int main(int argc, char** argv) {
	cbuild_selfrebuild(argc, argv);
	cbuild_proclist_t procs = {0};
	cbuild_cmd_t cmd = {0};
	if (cbuild_compare_mtime("main.o", "src/main.c") > 0) {
		cbuild_cmd_append_many(&cmd, CC, "-c", "src/main.c", "-o", "main.o");
		if (!cbuild_cmd_run(&cmd, .procs = &procs)) return 1;
		cmd.size = 0;
	}
	if (cbuild_compare_mtime("utils.o", "src/utils.c") > 0) {
		cbuild_cmd_append_many(&cmd, CC, "-c", "src/utils.c", "-o", "utils.o");
		if (!cbuild_cmd_run(&cmd, .procs = &procs)) return 1;
		cmd.size = 0;
	}
	cbuild_procs_wait(procs);
	cbuild_cmd_append_many(&cmd, CC, "-o", "app.run", "utils.o", "main.o");
	if (!cbuild_cmd_run(&cmd)) return 1;
	cbuild_proclist_clear(&procs);
	cbuild_cmd_clear(&cmd);
	return 0;
}
