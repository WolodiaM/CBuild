int main(void) {
	cbuild_sb_t sb = {0};
	cbuild_da_append(&sb, 'a');
	cbuild_da_append(&sb, 'b');
	char c0 = *cbuild_span_get(&sb, 0);
	TEST_ASSERT_EQ(c0, 'a', "Wrong element read at index 0"TEST_EXPECT_MSG(c), 'a', c0);
	char c1 = *cbuild_span_get(&sb, 1);
	TEST_ASSERT_EQ(c1, 'b', "Wrong element read at index 1"TEST_EXPECT_MSG(c), 'b', c1);
	cbuild_da_clear(&sb);
	return 0;
}
