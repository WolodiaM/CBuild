int main(void) {
	cbuild_sb_t sb = {0};
	cbuild_sb_append(&sb, 'a');
	cbuild_sb_append(&sb, 'b');
	TEST_ASSERT_EQ(*cbuild_sb_get(&sb, 0), 'a',
		"Wrong element read at index 0"TEST_EXPECT_MSG(c),
		'a', *cbuild_sb_get(&sb, 0));
	TEST_ASSERT_EQ(*cbuild_sb_get(&sb, 1), 'b',
		"Wrong element read at index 1"TEST_EXPECT_MSG(c),
		'b', *cbuild_sb_get(&sb, 1));
	cbuild_sb_clear(&sb);
	return 0;
}
