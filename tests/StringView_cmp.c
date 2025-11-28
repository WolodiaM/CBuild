int main(void) {
	cbuild_sv_t sv1  = cbuild_sv_from_lit("abc");
	cbuild_sv_t sv2  = cbuild_sv_from_lit("abc");
	cbuild_sv_t sv3  = cbuild_sv_from_lit("ABC");
	cbuild_sv_t sv4  = cbuild_sv_from_lit("ABC");
	cbuild_sv_t sv5  = cbuild_sv_from_lit("def");
	cbuild_sv_t sv6  = cbuild_sv_from_lit("defg");
	printf("%zu==%zu\n",sv1.size,sv6.size);
	// cbuild_sv_cmp
	int r1 = cbuild_sv_cmp(sv1, sv1);
	TEST_ASSERT_EQ(r1, 0,
		"cbuild_sv_cmp: Comparison with self failed"
		TEST_EXPECT_MSG(d), 0, r1);
	int r2 = cbuild_sv_cmp(sv1, sv2);
	TEST_ASSERT_EQ(r2, 0,
		"cbuild_sv_cmp: Comparison with identical string failed"
		TEST_EXPECT_MSG(d), 0, r2);
	int r3 = cbuild_sv_cmp(sv1, sv4);
	TEST_ASSERT_NEQ(r3, 0,
		"cbuild_sv_cmp: Comparison with same string (different case) should not be 0"
		TEST_EXPECT_MSG(d), 0, r3);
	int r4 = cbuild_sv_cmp(sv1, sv5);
	TEST_ASSERT_EQ(r4, -1,
		"cbuild_sv_cmp: Wrong difference reported when c1 < c2"
		TEST_EXPECT_MSG(d), -1, r4);
	int r5 = cbuild_sv_cmp(sv5, sv1);
	TEST_ASSERT_EQ(r5, 1,
		"cbuild_sv_cmp: Wrong difference reported when c1 > c2"
		TEST_EXPECT_MSG(d), 1, r5);
	int r6 = cbuild_sv_cmp(sv1, sv6);
	TEST_ASSERT_EQ(r6, -2,
		"cbuild_sv_cmp: Wrong difference reported when size1 < size2"
		TEST_EXPECT_MSG(d), -2, r6);
	int r7 = cbuild_sv_cmp(sv6, sv1);
	TEST_ASSERT_EQ(r7, 2,
		"cbuild_sv_cmp: Wrong difference reported when size1 > size2"
		TEST_EXPECT_MSG(d), 2, r7);
	// cbuild_sv_cmp_icase
	int r8 = cbuild_sv_cmp_icase(sv5, sv5);
	TEST_ASSERT_EQ(r8, 0,
		"cbuild_sv_cmp_icase: Comparison with self failed"
		TEST_EXPECT_MSG(d), 0, r8);
	int r9 = cbuild_sv_cmp_icase(sv1, sv2);
	TEST_ASSERT_EQ(r9, 0,
		"cbuild_sv_cmp_icase: Comparison with identical string failed"
		TEST_EXPECT_MSG(d), 0, r9);
	int r10 = cbuild_sv_cmp_icase(sv1, sv3);
	TEST_ASSERT_EQ(r10, 0,
		"cbuild_sv_cmp_icase: Comparison with same string (different case) failed"
		TEST_EXPECT_MSG(d), 0, r10);
	int r11 = cbuild_sv_cmp_icase(sv1, sv5);
	TEST_ASSERT_EQ(r11, -1,
		"cbuild_sv_cmp_icase: Wrong difference reported when c1 < c2"
		TEST_EXPECT_MSG(d), -1, r11);
	int r12 = cbuild_sv_cmp_icase(sv5, sv1);
	TEST_ASSERT_EQ(r12, 1,
		"cbuild_sv_cmp_icase: Wrong difference reported when c1 > c2"
		TEST_EXPECT_MSG(d), 1, r12);
	int r13 = cbuild_sv_cmp_icase(sv1, sv6);
	TEST_ASSERT_EQ(r13, -2,
		"cbuild_sv_cmp_icase: Wrong difference reported when size1 < size2"
		TEST_EXPECT_MSG(d),	-2, r13);
	int r14 = cbuild_sv_cmp_icase(sv6, sv1);
	TEST_ASSERT_EQ(r14, 2,
		"cbuild_sv_cmp_icase: Wrong difference reported when size1 > size2"
		TEST_EXPECT_MSG(d), 2, r14);
	return 0;
}
