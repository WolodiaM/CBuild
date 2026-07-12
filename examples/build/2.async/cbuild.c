#define CBUILD_IMPLEMENTATION
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "cbuild.h"
int main(int argc, char** argv) {
	cbuild_selfrebuild(argc, argv);
	cbuild_cmd_t cmd = {0};
	cbuild_proclist_t procs = {0};
	cbuild_da_append_many(&cmd, CC, "-c", "src/main.c", "-o", "main.o");
	cbuild_cmd_run(&cmd, .procs = &procs);
	cbuild_da_append_many(&cmd, CC, "-c", "src/utils.c", "-o", "utils.o");
	cbuild_cmd_run(&cmd, .procs = &procs);
	if (!cbuild_procs_wait(procs)) return 1;
	cbuild_da_append_many(&cmd, CC, "-o", "app.run", "utils.o", "main.o");
	if (!cbuild_cmd_run(&cmd)) return 1;
	cbuild_da_clear(&procs);
	cbuild_da_clear(&cmd);
	return 0;
}
