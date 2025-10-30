int main(void) {
	cbuild_sb_t sb = {0};
	char arr[] = { 'a', 'b', 'c' };
	cbuild_sb_append_arr(&sb, arr, 2);
	TEST_ASSERT_EQ(sb.size, 2,
		"Wrong element count after insertion" TEST_EXPECT_MSG(zu),
		(size_t)2, sb.size);
	TEST_ASSERT_EQ(sb.data[0], 'a',
		"Wrong element at index 0"TEST_EXPECT_MSG(c), 'a', sb.data[0]);
	TEST_ASSERT_EQ(sb.data[1], 'b',
		"Wrong element at index 1"TEST_EXPECT_MSG(c), 'b', sb.data[1]);
	cbuild_sb_clear(&sb);
	return 0;
}
