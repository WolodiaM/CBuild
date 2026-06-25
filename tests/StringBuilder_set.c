int main(void) {
	cbuild_sb_t sb = {0};
	cbuild_da_resize(&sb, 10);
	sb.size = 2;
	cbuild_span_set(&sb, 0, 'a');
	TEST_ASSERT_EQ(sb.data[0], 'a',
		"Wrong element after set"TEST_EXPECT_MSG(c), 'a', sb.data[0]);
	cbuild_da_clear(&sb);
	return 0;
}
