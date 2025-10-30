int main(void) {
	cbuild_sb_t sb = {0};
	cbuild_sb_appendf(&sb, "%d: %s", 2, "Hello, world!");
	cbuild_sb_append_null(&sb);
	TEST_ASSERT_EQ(sb.size, 17,
		"Wrong size after all appends"TEST_EXPECT_MSG(zu), (size_t)17, sb.size);
	TEST_ASSERT_STREQ(sb.data, "2: Hello, world!",
		"Wrong elements after all appends"TEST_EXPECT_MSG(s),
		"2: Hello, world!", sb.data);
	cbuild_sb_clear(&sb);
	char long_fmt[1024];
	memset(long_fmt, 'x', 1023);
	long_fmt[1023] = '\0';
	cbuild_sb_appendf(&sb, "%s", long_fmt);
	cbuild_sb_append_null(&sb);
	TEST_ASSERT_EQ(sb.size, 1024,
		"Long string append failed"TEST_EXPECT_MSG(zu),	(size_t)1024, sb.size);
	TEST_ASSERT_MEMEQ(sb.data, long_fmt, 1023,
		"Wrong data read after append of very long string.");
	cbuild_sb_clear(&sb);
	return 0;
}
