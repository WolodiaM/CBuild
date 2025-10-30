int main(void) {
	cbuild_sb_t sb = {0};
	const char* str = "abc";
	cbuild_sv_t sv = cbuild_sv_from_cstr(str);
	sv.size++;
	cbuild_sb_append_sv(&sb, sv);
	TEST_ASSERT_EQ(sb.size, sv.size,
		"Wrong number of elements appended" TEST_EXPECT_MSG(zu), sb.size, sv.size);
	// Could be done only because it is baked by C-string and size included '\0'
	TEST_ASSERT_STREQ(sb.data, sv.data,
		"Wrong string appended"TEST_EXPECT_MSG(s), sb.data, sv.data)
	cbuild_sv_t sv_out = cbuild_sb_to_sv(&sb);
	TEST_ASSERT_EQ(sv.size, sv_out.size,
		"Wrong number of elements after transformations (sv->sb->sv)"
		TEST_EXPECT_MSG(zu), sv.size, sv_out.size)
	TEST_ASSERT_STREQ(sv.data, sv_out.data,
		"Wrong elements after transformations (sv->sb->sv)"
		TEST_EXPECT_MSG(s), sv.data, sv_out.data)
	cbuild_sb_clear(&sb);
	sb = cbuild_sv_to_sb(sv);
	TEST_ASSERT_EQ(sb.size, sv.size,
		"Wrong number of elements set"TEST_EXPECT_MSG(zu), sb.size, sv.size);
	// Could be done only because it is backed by C-string and size
	// included '\0'
	TEST_ASSERT_STREQ(sb.data, sv.data,
		"Wrong string set"TEST_EXPECT_MSG(s), sb.data, sv.data)
	cbuild_sb_clear(&sb);
	return 0;
}
