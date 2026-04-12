#define CBUILD_IMPLEMENTATION
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "cbuild.h"
int main(int argc, char** argv) {
	cbuild_selfrebuild(argc, argv);
	cbuild_cmd_t cmd = {0};
	cbuild_cmd_append_many(&cmd, CC, "-o", "app.run", "src/main.c");
	if (!cbuild_cmd_run(&cmd)) return 1;
	cbuild_cmd_clear(&cmd);
	return 0;
}
