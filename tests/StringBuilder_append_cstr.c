int main(void) {
	cbuild_sb_t sb = {0};
	const char* STR = "Hello, world!";
	cbuild_sb_append_cstr(&sb, STR);
	cbuild_sb_append_null(&sb);
	TEST_ASSERT_STREQ(sb.data, STR,
		"Wrong elements inserted as a string"TEST_EXPECT_MSG(s), STR, sb.data);
	cbuild_sb_clear(&sb);
	return 0;
}
