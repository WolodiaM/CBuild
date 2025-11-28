int main(void) {
	printf("%s\n", TEST_TEMP_FILE);
	cbuild_cmd_t c1 = {0};
	cbuild_cmd_append_many(&c1, "a", "b", "--flag", "-f",
		"val with spaces");
	cbuild_sb_t sb1 = cbuild_cmd_to_sb(c1);
	cbuild_sb_append_null(&sb1);
	char* cmp1 = "a b --flag -f \'val with spaces\'";
	TEST_ASSERT_STREQ(sb1.data, cmp1,
		"cbuild_cmd_to_sb: Failed to correctly quote arguments with spaces"
		TEST_EXPECT_RMSG(CBuildSBFmt), CBuildSBArg(sb1), cbuild_sv_from_cstr(cmp1));
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
		"cbuild_cmd_to_sb: Failed to correctly render a typical compiler command"
		TEST_EXPECT_RMSG(CBuildSBFmt), CBuildSBArg(sb1), cbuild_sv_from_cstr(cmp2));
	cbuild_sb_clear(&sb1);
	cbuild_cmd_clear(&c1);
	return 0;
}
