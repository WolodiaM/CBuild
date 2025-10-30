int main(void) {
	cbuild_sb_t sb = {0};
	cbuild_sb_append_many(&sb, 'a', 'b', 'c', 'd');
	cbuild_sb_remove(&sb, 1);
	TEST_ASSERT_EQ(sb.data[0], 'a',
		"Wrong element at index 0"TEST_EXPECT_MSG(c), 'a', sb.data[0]);
	TEST_ASSERT_EQ(sb.data[1], 'c',
		"Wrong element at index 1"TEST_EXPECT_MSG(c), 'c', sb.data[1]);
	TEST_ASSERT_EQ(sb.data[2], 'd',
		"Wrong element at index 2"TEST_EXPECT_MSG(c), 'd', sb.data[2]);
	cbuild_sb_clear(&sb);
	return 0;
}
