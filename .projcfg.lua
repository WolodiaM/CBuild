local pc = require("projcfg");
return {
	{
		CFG = {
			ctags = {
				exclude = { "changelog/*", "tests/*", "wiki/*", "wikimk/*", "build/*", "*.json", "*.md", "cbuild.h" },
				kinds = { c = "+px" },
				map = { c = { "+.h" } },
			},
		},
	},
	{
		pattern = ".*%.[ch]",
		CFG = {
			linter = {
				args = {
					"-xc",
					"-std=gnu99",
					"-Wall",
					"-Wextra",
					"-Wno-comments",
					"-Wconversion",
					"-Wcast-align",
					"-Wvla",
					"-Wno-override-init",
					"-Wshadow",
					"-Wno-pragma-once-outside-header",
				},
			},
		},
	},
	{
		pattern = "tests/.*%.c",
		CFG = {
			linter = {
				args = pc.extend({
					"-include", "framework.h",
					"-DTEST_RUN_PLATFORM=\"\"",
				}),
			},
		},
	},
	{
		pattern = { "cbuild.h", "cbuild.split.h" },
		CFG = {
			linter = {
				args = pc.extend({
					"-DCBUILD_IMPLEMENTATION",
				}),
			},
		},
	},
	{
		pattern = "src/.*%.[ch]",
		CFG = {
			ctags = {
				check = true,
			},
		},
	},
};
