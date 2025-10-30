int main(void) {
	cbuild_sb_t sb = {0};
	// Manual resize
	cbuild_sb_resize(&sb, 256);
	TEST_ASSERT_EQ(sb.capacity, 256,
		"Wrong capacity after allocation"TEST_EXPECT_MSG(zu),
		(size_t)256, sb.capacity);
	TEST_ASSERT_NEQ(sb.data, NULL, "Pointer is NULL after allocation.");
	cbuild_sb_clear(&sb);
	TEST_ASSERT_EQ(sb.capacity, 0,
		"Wrong capacity after allocation"TEST_EXPECT_MSG(zu),
		(size_t)0, sb.capacity);
	TEST_ASSERT_EQ(sb.data, NULL, "Pointer is non-NULL after allocation.");
	// Automatic resize
	cbuild_sb_append(&sb, 'a');
	cbuild_sb_append(&sb, 'a');
	cbuild_sb_append(&sb, 'a');
	TEST_ASSERT_EQ(sb.capacity, 4,
		"Wrong capacity after allocation"TEST_EXPECT_MSG(zu),
		(size_t)4, sb.capacity);
	cbuild_sb_clear(&sb);
	return 0;
}
