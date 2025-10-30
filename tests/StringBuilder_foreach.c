int main(void) {
	cbuild_sb_t sb = {0};
	cbuild_sb_append_many(&sb, 'a', 'b', 'c', 'd');
	char found = 'a' - 1;
	cbuild_sb_foreach(&sb, elem) {
		TEST_ASSERT_EQ(*elem, found+1,
			"Wrong element at index %zu"TEST_EXPECT_MSG(c),
			elem - sb.data, found + 1, *elem);
		found++;
	}
	cbuild_sb_clear(&sb);
	return 0;
}
