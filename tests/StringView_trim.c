int main(void) {
	const char* str1 = "   \t\t\tabc";
	cbuild_sv_t sv1  = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv2  = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv3  = cbuild_sv_from_cstr(str1);
	const char* str2 = "abc \t \t \t";
	cbuild_sv_t sv4  = cbuild_sv_from_cstr(str2);
	cbuild_sv_t sv5  = cbuild_sv_from_cstr(str2);
	cbuild_sv_t sv6  = cbuild_sv_from_cstr(str2);
	const char* str3 = " \t abc\t \t";
	cbuild_sv_t sv7  = cbuild_sv_from_cstr(str3);
	cbuild_sv_t sv8  = cbuild_sv_from_cstr(str3);
	cbuild_sv_t sv9  = cbuild_sv_from_cstr(str3);
	const char* str4 = "    \t\t    ";
	cbuild_sv_t sv10 = cbuild_sv_from_cstr(str4);
	cbuild_sv_t sv11 = cbuild_sv_from_cstr(str4);
	cbuild_sv_t sv12 = cbuild_sv_from_cstr(str4);
	size_t r1 = cbuild_sv_trim_left(&sv1);
	// cbuild_sv_trim_left
	TEST_ASSERT_EQ(r1, 6,
		"cbuild_sv_trim_left(str1): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)6, r1);
	char fmt1[20] = {0};
	sprintf(fmt1, CBuildSVFmt, CBuildSVArg(sv1));
	TEST_ASSERT_STREQ(fmt1, "abc",
		"cbuild_sv_trim_left(str1): Wrong data after trim"
		TEST_EXPECT_MSG(s), "abc", fmt1);
	size_t r4 = cbuild_sv_trim_left(&sv4);
	TEST_ASSERT_EQ(r4, 0,
		"cbuild_sv_trim_left(str2): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)0, r4);
	char fmt4[20] = {0};
	sprintf(fmt4, CBuildSVFmt, CBuildSVArg(sv4));
	TEST_ASSERT_STREQ(fmt4, "abc \t \t \t",
		"cbuild_sv_trim_left(str2): Wrong data after trim"
		TEST_EXPECT_MSG(s), "abc \t \t \t", fmt4);
	size_t r7 = cbuild_sv_trim_left(&sv7);
	TEST_ASSERT_EQ(r7, 3,
		"cbuild_sv_trim_left(str3): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)3, r7);
	char fmt7[20] = {0};
	sprintf(fmt7, CBuildSVFmt, CBuildSVArg(sv7));
	TEST_ASSERT_STREQ(fmt7, "abc\t \t",
		"cbuild_sv_trim_left(str3): Wrong data after trim"
		TEST_EXPECT_MSG(s), "abc\t \t", fmt7);
	size_t r10 = cbuild_sv_trim_left(&sv10);
	TEST_ASSERT_EQ(r10, 10,
		"cbuild_sv_trim_left(str4): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)10, r10);
	char fmt10[20] = {0};
	sprintf(fmt10, CBuildSVFmt, CBuildSVArg(sv10));
	TEST_ASSERT_STREQ(fmt10, "",
		"cbuild_sv_trim_left(str4): Wrong data after trim"
		TEST_EXPECT_MSG(s), "",	fmt10);
	// cbuild_sv_trim_right
	size_t r2 = cbuild_sv_trim_right(&sv2);
	TEST_ASSERT_EQ(r2, 0,
		"cbuild_sv_trim_right(str1): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)0, r2);
	char fmt2[20] = {0};
	sprintf(fmt2, CBuildSVFmt, CBuildSVArg(sv2));
	TEST_ASSERT_STREQ(fmt2, "   \t\t\tabc",
		"cbuild_sv_trim_right(str1): Wrong data after trim"
		TEST_EXPECT_MSG(s), "   \t\t\tabc", fmt2);
	size_t r5 = cbuild_sv_trim_right(&sv5);
	TEST_ASSERT_EQ(r5, 6,
		"cbuild_sv_trim_right(str2): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)6, cbuild_sv_trim_right(&sv5));
	char fmt5[20] = {0};
	sprintf(fmt5, CBuildSVFmt, CBuildSVArg(sv5));
	TEST_ASSERT_STREQ(fmt5, "abc",
		"cbuild_sv_trim_right(str2): Wrong data after trim"
		TEST_EXPECT_MSG(s), "abc", fmt5);
	size_t r8 = cbuild_sv_trim_right(&sv8);
	TEST_ASSERT_EQ(r8, 3,
		"cbuild_sv_trim_right(str3): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)3, cbuild_sv_trim_right(&sv8));
	char fmt8[20] = {0};
	sprintf(fmt8, CBuildSVFmt, CBuildSVArg(sv8));
	TEST_ASSERT_STREQ(fmt8, " \t abc",
		"cbuild_sv_trim_right(str3): Wrong data after trim"
		TEST_EXPECT_MSG(s), " \t abc", fmt8);
	size_t r11 = cbuild_sv_trim_right(&sv11);
	TEST_ASSERT_EQ(r11, 10,
		"cbuild_sv_trim_right(str4): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)10, cbuild_sv_trim_right(&sv11));
	char fmt11[20] = {0};
	sprintf(fmt11, CBuildSVFmt, CBuildSVArg(sv11));
	TEST_ASSERT_STREQ(fmt11, "",
		"cbuild_sv_trim_right(str4): Wrong data after trim"
		TEST_EXPECT_MSG(s), "", fmt11);
	// cbuild_sv_trim
	size_t r3 = cbuild_sv_trim(&sv3);
	TEST_ASSERT_EQ(r3, 6,
		"cbuild_sv_trim(str1): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)6, cbuild_sv_trim(&sv3));
	char fmt3[20] = {0};
	sprintf(fmt3, CBuildSVFmt, CBuildSVArg(sv3));
	TEST_ASSERT_STREQ(fmt3, "abc",
		"cbuild_sv_trim(str1): Wrong data after trim"
		TEST_EXPECT_MSG(s), "abc", fmt3);
	size_t r6 = cbuild_sv_trim(&sv6);
	TEST_ASSERT_EQ(r6, 6,
		"cbuild_sv_trim(str2): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)6, cbuild_sv_trim(&sv6));
	char fmt6[20] = {0};
	sprintf(fmt6, CBuildSVFmt, CBuildSVArg(sv6));
	TEST_ASSERT_STREQ(fmt6, "abc",
		"cbuild_sv_trim(str2): Wrong data after trim"
		TEST_EXPECT_MSG(s), "abc", fmt6);
	size_t r9 = cbuild_sv_trim(&sv9);
	TEST_ASSERT_EQ(r9, 6,
		"cbuild_sv_trim(str3): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)6, cbuild_sv_trim(&sv9));
	char fmt9[20] = {0};
	sprintf(fmt9, CBuildSVFmt, CBuildSVArg(sv9));
	TEST_ASSERT_STREQ(fmt9, "abc",
		"cbuild_sv_trim(str3): Wrong data after trim"
		TEST_EXPECT_MSG(s), "abc", fmt9);
	size_t r12 = cbuild_sv_trim(&sv12);
	TEST_ASSERT_EQ(r12, 10,
		"cbuild_sv_trim(str4): Wrong number of characters trimmed"
		TEST_EXPECT_MSG(zu), (size_t)10, cbuild_sv_trim(&sv12));
	char fmt12[20] = {0};
	sprintf(fmt12, CBuildSVFmt, CBuildSVArg(sv12));
	TEST_ASSERT_STREQ(fmt12, "",
		"cbuild_sv_trim(str4): Wrong data after trim"
		TEST_EXPECT_MSG(s), "", fmt12);
	return 0;
}
