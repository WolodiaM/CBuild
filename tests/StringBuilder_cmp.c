int main(void) {
	// Prepare string builders
	const char* str1 = "abc";
	cbuild_sb_t sb1  = {0};
	cbuild_sb_append_cstr(&sb1, str1);
	const char* str2 = "abc";
	cbuild_sb_t sb2  = {0};
	cbuild_sb_append_cstr(&sb2, str2);
	const char* str3 = "ABC";
	cbuild_sb_t sb3  = {0};
	cbuild_sb_append_cstr(&sb3, str3);
	const char* str4 = "ABC";
	cbuild_sb_t sb4  = {0};
	cbuild_sb_append_cstr(&sb4, str4);
	const char* str5 = "def";
	cbuild_sb_t sb5  = {0};
	cbuild_sb_append_cstr(&sb5, str5);
	const char* str6 = "defg";
	cbuild_sb_t sb6  = {0};
	cbuild_sb_append_cstr(&sb6, str6);
	// Standard compare
	int r1 = cbuild_sb_cmp(&sb1, &sb1);
	TEST_ASSERT_EQ(r1, 0,
		"cbuild_sb_cmp: Comparison with self failed"
		TEST_EXPECT_MSG(d), 0, r1);
	int r2 = cbuild_sb_cmp(&sb1, &sb2);
	TEST_ASSERT_EQ(r2, 0,
		"cbuild_sb_cmp: Comparison with identical string failed"
		TEST_EXPECT_MSG(d), 0, r2);
	int r3 = cbuild_sb_cmp(&sb1, &sb4);
	TEST_ASSERT_NEQ(r3, 0,
		"cbuild_sb_cmp: Comparison with same string (different case) should not be 0"
		TEST_EXPECT_MSG(d), 0, r3);
	int r4 = cbuild_sb_cmp(&sb1, &sb5);
	TEST_ASSERT_EQ(r4, -1,
		"cbuild_sb_cmp: Wrong difference reported when c1 < c2"
		TEST_EXPECT_MSG(d), -1, r4);
	int r5 = cbuild_sb_cmp(&sb5, &sb1);
	TEST_ASSERT_EQ(r5, 1,
		"cbuild_sb_cmp: Wrong difference reported when c1 > c2"
		TEST_EXPECT_MSG(d), 1, r5);
	int r6 = cbuild_sb_cmp(&sb1, &sb6);
	TEST_ASSERT_EQ(r6, -2,
		"cbuild_sb_cmp: Wrong difference reported when size1 < size2"
		TEST_EXPECT_MSG(d), -2, r6);
	int r7 = cbuild_sb_cmp(&sb6, &sb1);
	TEST_ASSERT_EQ(r7, 2,
		"cbuild_sb_cmp: Wrong difference reported when size1 > size2"
		TEST_EXPECT_MSG(d), 2, r7);
	// Case-less compare
	TEST_ASSERT_EQ(cbuild_sb_cmp_icase(&sb5, &sb5), 0,
		"cbuild_sb_cmp_icase: Comparison with self failed"
		TEST_EXPECT_MSG(d), 0,
		cbuild_sb_cmp_icase(&sb5, &sb5));
	int r8 = cbuild_sb_cmp_icase(&sb1, &sb2);
	TEST_ASSERT_EQ(r8, 0,
		"cbuild_sb_cmp_icase: Comparison with identical string failed"
		TEST_EXPECT_MSG(d), 0, r8);
	int r9 = cbuild_sb_cmp_icase(&sb1, &sb3);
	TEST_ASSERT_EQ(r9, 0,
		"cbuild_sb_cmp_icase: Comparison with same string (different case) failed"
		TEST_EXPECT_MSG(d), 0, r8);
	int r10 = cbuild_sb_cmp_icase(&sb1, &sb5);
	TEST_ASSERT_EQ(r10, -1,
		"cbuild_sb_cmp_icase: Wrong difference reported when c1 < c2"
		TEST_EXPECT_MSG(d), -1, r10);
	int r11 = cbuild_sb_cmp_icase(&sb5, &sb1);
	TEST_ASSERT_EQ(r11, 1,
		"cbuild_sb_cmp_icase: Wrong difference reported when c1 > c2"
		TEST_EXPECT_MSG(d), 1, r11);
	int r12 = cbuild_sb_cmp_icase(&sb1, &sb6);
	TEST_ASSERT_EQ(r12, -2,
		"cbuild_sb_cmp_icase: Wrong difference reported when size1 < size2"
		TEST_EXPECT_MSG(d), -2, r12);
	int r13 = cbuild_sb_cmp_icase(&sb6, &sb1);
	TEST_ASSERT_EQ(r13, 2,
		"cbuild_sb_cmp_icase: Wrong difference reported when size1 > size2"
		TEST_EXPECT_MSG(d), 2, r13);
	// Clear all string builders
	cbuild_sb_clear(&sb1);
	cbuild_sb_clear(&sb2);
	cbuild_sb_clear(&sb3);
	cbuild_sb_clear(&sb4);
	cbuild_sb_clear(&sb5);
	cbuild_sb_clear(&sb6);
}
