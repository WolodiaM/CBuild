bool tdelim1(const cbuild_sv_t* sv, size_t idx, void* args) {
	CBUILD_UNUSED(args);
	if(idx > 4 && sv->data[idx] == '|') {
		return true;
	}
	return false;
}
bool tdelim2(const cbuild_sv_t* sv, size_t idx, void* args) {
	CBUILD_UNUSED(sv);
	CBUILD_UNUSED(idx);
	CBUILD_UNUSED(args);
	return false;
}
int main(void) {
	const char* str1 = "abc|def|-|ghj";
	// cbuild_sv_chop
	cbuild_sv_t sv1 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv2 = cbuild_sv_chop(&sv1, 3);
	cbuild_sv_t sv3 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv4 = cbuild_sv_chop(&sv3, 40);
	cbuild_sv_t cmp1 = cbuild_sv_from_cstr("|def|-|ghj");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv1, cmp1), 0,
		"Wrong part left after cbuild_sv_chop"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp1), CBuildSVArg(sv1));
	cbuild_sv_t cmp2 = cbuild_sv_from_cstr("abc");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv2, cmp2), 0,
		"Wrong part chopped after cbuild_sv_chop"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp2), CBuildSVArg(sv2));
	TEST_ASSERT_EQ(
		cbuild_sv_cmp(sv3, cbuild_sv_from_cstr("")), 0,
		"Wrong part left after chopping (on overflow) after cbuild_sv_chop"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv3));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv4, cbuild_sv_from_cstr(str1)), 0,
		"Wrong part chopped (on overflow) after cbuild_sv_chop"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr(str1)), CBuildSVArg(sv4));
	// cbuild_sv_chop_by_func
	cbuild_sv_t sv5 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv6 = cbuild_sv_chop_by_func(&sv5, tdelim1, NULL);
	cbuild_sv_t sv7 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv8 = cbuild_sv_chop_by_func(&sv7, tdelim2, NULL);
	cbuild_sv_t cmp3 = cbuild_sv_from_cstr("-|ghj");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv5, cmp3), 0,
		"Wrong part left after cbuild_sv_chop_by_func"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp3), CBuildSVArg(sv5));
	cbuild_sv_t cmp4 = cbuild_sv_from_cstr("abc|def");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv6, cmp4), 0,
		"Wrong part chopped after cbuild_sv_chop_by_func"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp4), CBuildSVArg(sv6));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv7, cbuild_sv_from_cstr("")), 0,
		"Wrong part left after chopping (on overflow) after cbuild_sv_chop_by_func"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv7));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv8, cbuild_sv_from_cstr(str1)), 0,
		"Wrong part chopped (on overflow) after cbuild_sv_chop_by_func"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr(str1)), CBuildSVArg(sv8));
	// cbuild_sv_chop_by_delim
	cbuild_sv_t sv9  = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv10 = cbuild_sv_chop_by_delim(&sv9, '|');
	cbuild_sv_t sv11 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv12 = cbuild_sv_chop_by_delim(&sv11, '/');
	cbuild_sv_t cmp5 = cbuild_sv_from_cstr("def|-|ghj");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv9, cmp5), 0,
		"Wrong part left after cbuild_sv_chop_by_delim"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp5), CBuildSVArg(sv9));
	cbuild_sv_t cmp6 = cbuild_sv_from_cstr("abc");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv10, cmp6), 0,
		"Wrong part chopped after cbuild_sv_chop_by_delim"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp6), CBuildSVArg(sv10));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv11, cbuild_sv_from_cstr("")), 0,
		"Wrong part left after chopping (on overflow) on cbuild_sv_chop_by_delim"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv11));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv12, cbuild_sv_from_cstr(str1)), 0,
		"Wrong part chopped (on overflow) on cbuild_sv_chop_by_delim"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr(str1)), CBuildSVArg(sv12));
	// cbuild_sv_chop_by_sv
	cbuild_sv_t sv13 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv14 = cbuild_sv_chop_by_sv(&sv13, cbuild_sv_from_cstr("|-|"));
	cbuild_sv_t sv15 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv16 = cbuild_sv_chop_by_sv(&sv15, cbuild_sv_from_cstr("---"));
	cbuild_sv_t cmp7 = cbuild_sv_from_cstr("ghj");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv13, cmp7), 0,
		"Wrong part left after cbuild_sv_chop_by_sv"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp7), CBuildSVArg(sv13));
	cbuild_sv_t cmp8 = cbuild_sv_from_cstr("abc|def");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv14, cmp8), 0,
		"Wrong part chopped after cbuild_sv_chop_by_sv"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp8), CBuildSVArg(sv14));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv15, cbuild_sv_from_cstr("")), 0,
		"Wrong part left after chopping (on overflow) after cbuild_sv_chop_by_sv"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv15));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv16, cbuild_sv_from_cstr(str1)), 0,
		"Wrong part chopped (on overflow) after cbuild_sv_chop_by_sv"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr(str1)), CBuildSVArg(sv16));
	// Some more chops for some edge-cases
	// Delimeter is part of long sequence of same chars
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("--");
		cbuild_sv_t part  = cbuild_sv_chop_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("foo")), 0,
			"Multi-char delim chop from long sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("foo")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("-bar---baz")), 0,
			"Multi-char delim chop from long sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("-bar---baz")),
			CBuildSVArg(sv));
	}
	// Delimeter is a single character
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("-");
		cbuild_sv_t part  = cbuild_sv_chop_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("foo")), 0,
			"Single-char delim chop from long sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("foo")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("--bar---baz")), 0,
			"Single-char delim chop from long sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("--bar---baz")),
			CBuildSVArg(sv));
	}
	return 0;
}
