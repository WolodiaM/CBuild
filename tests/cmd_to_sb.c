#include "framework.h"
int main(void) {
	cbuild_cmd_t c1 = {0};
	cbuild_cmd_append_many(&c1, "a", "b", "--flag", "-f",
		"val with spaces");
	cbuild_sb_t sb1 = cbuild_cmd_to_sb(c1);
	cbuild_sb_append_null(&sb1);
	char* cmp1 = "a b --flag -f \'val with spaces\'";
	TEST_ASSERT_STREQ(sb1.data, cmp1,
		"Get: \"" CBuildSBFmt "\", expected: \"%s\"",
		CBuildSBArg(sb1), cmp1);
	cbuild_cmd_clear(&c1);
	cbuild_cmd_append_many(&c1, CBUILD_CC,
		CBUILD_CARGS_DEFINE_VAL("VER", "1.0"),
		CBUILD_CARGS_INCLUDE("common.h"),
		"file with spaces.c", "-o", "file.run");
	cbuild_sb_clear(&sb1);
	sb1 = cbuild_cmd_to_sb(c1);
	cbuild_sb_append_null(&sb1);
	char* cmp2 =
		CBUILD_CC " -DVER=1.0 --include common.h 'file with spaces.c' -o file.run";
	TEST_ASSERT_STREQ(sb1.data, cmp2,
		"Get: \"" CBuildSBFmt "\", expected: \"%s\"",
		CBuildSBArg(sb1), cmp2);
	cbuild_sb_clear(&sb1);
	cbuild_cmd_clear(&c1);
	return 0;
}
