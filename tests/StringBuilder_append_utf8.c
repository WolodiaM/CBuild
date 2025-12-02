int main(void) {
	cbuild_sb_t sb = {0};
	const char* test = "aф€😀";
	cbuild_sb_append_utf8(&sb, 'a');
	cbuild_sb_append_utf8(&sb, 1092);
	cbuild_sb_append_utf8(&sb, 8364);
	cbuild_sb_append_utf8(&sb, 128512);
	cbuild_sb_append_null(&sb);
	TEST_ASSERT_EQ(sb.size, 11,
		"Wrong size after appends"TEST_EXPECT_MSG(zu), (size_t)11, sb.size);
	TEST_ASSERT_MEMEQ(sb.data, test, 10,
		"Error while appending utf8 characters to a string builder"
		TEST_EXPECT_MSG(s), test, sb.data);
	cbuild_sb_t copy = {0};
	cbuild_sb_append_arr(&copy, sb.data, sb.size);
	cbuild_sb_t different = {0};
	cbuild_sb_append_cstr(&different, "abc");
	cbuild_sb_append_null(&different);
	TEST_ASSERT_EQ(cbuild_sb_cmp(&sb, &copy), 0,
		"Same utf8 string reported as different in comparisons.");
	TEST_ASSERT_NEQ(cbuild_sb_cmp(&sb, &different), 0,
		"Different utf8 string reported as same in comparisons.");
	size_t len = cbuild_sb_utf8len(&sb);
	TEST_ASSERT_EQ(len, 5,
		"Wrong codepoint count calculated"TEST_EXPECT_MSG(zu), (size_t)5, len);
	cbuild_sb_clear(&sb);
	cbuild_sb_clear(&copy);
	cbuild_sb_clear(&different);
	return 0;
}
