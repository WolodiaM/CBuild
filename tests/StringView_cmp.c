int main(void) {
	const char* str1 = "abc";
	cbuild_sv_t sv1  = cbuild_sv_from_lit(str1);
	const char* str2 = "abc";
	cbuild_sv_t sv2  = cbuild_sv_from_lit(str2);
	const char* str3 = "ABC";
	cbuild_sv_t sv3  = cbuild_sv_from_lit(str3);
	const char* str4 = "ABC";
	cbuild_sv_t sv4  = cbuild_sv_from_lit(str4);
	const char* str5 = "def";
	cbuild_sv_t sv5  = cbuild_sv_from_lit(str5);
	const char* str6 = "defg";
	cbuild_sv_t sv6  = cbuild_sv_from_lit(str6);
	// cbuild_sv_cmp
	int r1 = cbuild_sv_cmp(sv1, sv1);
	TEST_ASSERT_EQ(r1, 0,
		"Comparison with self failed"TEST_EXPECT_MSG(d), 0, r1);
	int r2 = cbuild_sv_cmp(sv1, sv2);
	TEST_ASSERT_EQ(r2, 0,
		"Comparison with same string failed"TEST_EXPECT_MSG(d), 0, r2);
	int r3 = cbuild_sv_cmp(sv1, sv4);
	TEST_ASSERT_EQ(r3, 1,
		"Comparison with same string (different case) succeed"
		TEST_EXPECT_MSG(d), 1, r3);
	int r4 = cbuild_sv_cmp(sv1, sv5);
	TEST_ASSERT_EQ(r4, -1,
		"Wrong difference reported when c1 < c2"TEST_EXPECT_MSG(d), -1, r4);
	int r5 = cbuild_sv_cmp(sv5, sv1);
	TEST_ASSERT_EQ(r5, 1,
		"Wrong difference reported when c1 > c2"TEST_EXPECT_MSG(d), 1, r5);
	int r6 = cbuild_sv_cmp(sv1, sv6);
	TEST_ASSERT_EQ(r6, -2,
		"Wrong difference reported when size1 < size2"TEST_EXPECT_MSG(d), -2, r6);
	int r7 = cbuild_sv_cmp(sv6, sv1);
	TEST_ASSERT_EQ(r7, 2,
		"Wrong difference reported when size1 > size2"TEST_EXPECT_MSG(d), 2, r7);
	// cbuild_sv_cmp_icase
	int r8 = cbuild_sv_cmp_icase(sv5, sv5);
	TEST_ASSERT_EQ(r8, 0,
		"Case-less comparison with self failed"TEST_EXPECT_MSG(d), 0, r8);
	int r9 = cbuild_sv_cmp_icase(sv1, sv2);
	TEST_ASSERT_EQ(r9, 0,
		"Case-less comparison with same string failed"
		TEST_EXPECT_MSG(d), 0, r9);
	int r10 = cbuild_sv_cmp_icase(sv1, sv3);
	TEST_ASSERT_EQ(r10, 0,
		"Case-less comparison with same string (different case) failed"
		TEST_EXPECT_MSG(d), 0, r10);
	int r11 = cbuild_sv_cmp_icase(sv1, sv5);
	TEST_ASSERT_EQ(r11, -1,
		"Wrong difference reported when case-less c1 < c2"
		TEST_EXPECT_MSG(d), -1, r11);
	int r12 = cbuild_sv_cmp_icase(sv5, sv1);;;;
	TEST_ASSERT_EQ(r12, 1,
		"Wrong difference reported when case-less c1 > c2"
		TEST_EXPECT_MSG(d), 1, r12);
	int r13 = cbuild_sv_cmp_icase(sv1, sv6);
	TEST_ASSERT_EQ(r13, -2,
		"Wrong difference reported when case-less size1 < size2"
		TEST_EXPECT_MSG(d),	-2, r13);
	int r14 = cbuild_sv_cmp_icase(sv6, sv1);
	TEST_ASSERT_EQ(r14, 2,
		"Wrong difference reported when case-less size1 > size2"
		TEST_EXPECT_MSG(d), 2, r14);
	return 0;
}
