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
bool tdelim3(const cbuild_sv_t* sv, size_t idx, void* args) {
	CBUILD_UNUSED(args);
	if(idx < 8 && sv->data[idx] == '|') {
		return true;
	}
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
	// Delimiter is part of long sequence of same chars
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("--");
		cbuild_sv_t part  = cbuild_sv_chop_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("foo")), 0,
			"Multi-char delim cbuild_sv_chop_by_sv from long sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("foo")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("-bar---baz")), 0,
			"Multi-char delim cbuid_sv_chop_by_sv from long sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("-bar---baz")),
			CBuildSVArg(sv));
	}
	// Delimiter is a single character
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("-");
		cbuild_sv_t part  = cbuild_sv_chop_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("foo")), 0,
			"Single-char delim cbuild_sv_chop_by_sv from long sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("foo")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("--bar---baz")), 0,
			"Single-char delim cbuild_sv_chop_by_sv from long sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("--bar---baz")),
			CBuildSVArg(sv));
	}
	// Extreme cases - start of a string
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("foo");
		cbuild_sv_t part  = cbuild_sv_chop_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("")), 0,
			"Delim of cbuild_sv_chop_by_sv is at the start of sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("---bar---baz")), 0,
			"Delim of cbuild_sv_chop_by_sv is at the start of sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("---bar---baz")),
			CBuildSVArg(sv));
	}
	// Extreme cases - end of a string
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("baz");
		cbuild_sv_t part  = cbuild_sv_chop_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("foo---bar---")), 0,
			"Delim of cbuild_sv_chop_by_sv is at the end of sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("foo---bar---")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("")), 0,
			"Delim of cbuild_sv_chop_by_sv is at the end of sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("")),
			CBuildSVArg(sv));
	}
	// Extreme cases - full string
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr(str);
		cbuild_sv_t part  = cbuild_sv_chop_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("")), 0,
			"Delim of cbuild_sv_chop_by_sv is full sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("")), 0,
			"Delim of cbuild_sv_chop_by_sv is full sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("")),
			CBuildSVArg(sv));
	}
	// cbuild_sv_chop_right
	cbuild_sv_t sv17 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv18 = cbuild_sv_chop_right(&sv17, 3);
	cbuild_sv_t sv19 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv20 = cbuild_sv_chop(&sv19, 40);
	cbuild_sv_t cmp9 = cbuild_sv_from_cstr("abc|def|-|");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv17, cmp9), 0,
		"Wrong part left after cbuild_sv_chop_right"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp9), CBuildSVArg(sv17));
	cbuild_sv_t cmp10 = cbuild_sv_from_cstr("ghj");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv18, cmp10), 0,
		"Wrong part chopped after cbuild_sv_chop_right"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp10), CBuildSVArg(sv18));
	TEST_ASSERT_EQ(
		cbuild_sv_cmp(sv19, cbuild_sv_from_cstr("")), 0,
		"Wrong part left after chopping (on overflow) after cbuild_sv_chop_right"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv3));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv20, cbuild_sv_from_cstr(str1)), 0,
		"Wrong part chopped (on overflow) after cbuild_sv_chop_right"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr(str1)), CBuildSVArg(sv20));
	// cbuild_sv_chop_right_by_delim
	cbuild_sv_t sv21 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv22 = cbuild_sv_chop_right_by_delim(&sv21, '|');
	cbuild_sv_t sv23 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv24 = cbuild_sv_chop_right_by_delim(&sv23, '/');
	cbuild_sv_t cmp11 = cbuild_sv_from_cstr("abc|def|-");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv21, cmp11), 0,
		"Wrong part left after cbuild_sv_chop_right_by_delim"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp11), CBuildSVArg(sv21));
	cbuild_sv_t cmp12 = cbuild_sv_from_cstr("ghj");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv22, cmp12), 0,
		"Wrong part chopped after cbuild_sv_chop_right_by_delim"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp12), CBuildSVArg(sv22));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv23, cbuild_sv_from_cstr("")), 0,
		"Wrong part left after chopping (on overflow) on cbuild_sv_chop_right_by_delim"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv23));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv24, cbuild_sv_from_cstr(str1)), 0,
		"Wrong part chopped (on overflow) on cbuild_sv_chop_right_by_delim"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr(str1)), CBuildSVArg(sv24));
	// cbuild_sv_chop_by_func
	cbuild_sv_t sv25 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv26 = cbuild_sv_chop_right_by_func(&sv25, tdelim3, NULL);
	cbuild_sv_t sv27 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv28 = cbuild_sv_chop_right_by_func(&sv27, tdelim2, NULL);
	cbuild_sv_t cmp13 = cbuild_sv_from_cstr("abc|def");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv25, cmp13), 0,
		"Wrong part left after cbuild_sv_chop_right_by_func"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp13), CBuildSVArg(sv25));
	cbuild_sv_t cmp14 = cbuild_sv_from_cstr("-|ghj");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv26, cmp14), 0,
		"Wrong part chopped after cbuild_sv_chop_right_by_func"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp14), CBuildSVArg(sv26));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv27, cbuild_sv_from_cstr("")), 0,
		"Wrong part left after chopping (on overflow) after cbuild_sv_chop_right_by_func"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv27));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv28, cbuild_sv_from_cstr(str1)), 0,
		"Wrong part chopped (on overflow) after cbuild_sv_chop_right_by_func"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr(str1)), CBuildSVArg(sv28));
	// cbuild_sv_chop_by_sv
	cbuild_sv_t sv29 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv30 = cbuild_sv_chop_right_by_sv(&sv29, cbuild_sv_from_cstr("|-|"));
	cbuild_sv_t sv31 = cbuild_sv_from_cstr(str1);
	cbuild_sv_t sv32 = cbuild_sv_chop_right_by_sv(&sv31, cbuild_sv_from_cstr("---"));
	cbuild_sv_t cmp15 = cbuild_sv_from_cstr("abc|def");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv29, cmp15), 0,
		"Wrong part left after cbuild_sv_chop_right_by_sv"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp15), CBuildSVArg(sv29));
	cbuild_sv_t cmp16 = cbuild_sv_from_cstr("ghj");
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv30, cmp16), 0,
		"Wrong part chopped after cbuild_sv_chop_right_by_sv"TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cmp16), CBuildSVArg(sv30));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv31, cbuild_sv_from_cstr("")), 0,
		"Wrong part left after chopping (on overflow) after cbuild_sv_chop_right_by_sv"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv31));
	TEST_ASSERT_EQ(cbuild_sv_cmp(sv32, cbuild_sv_from_cstr(str1)), 0,
		"Wrong part chopped (on overflow) after cbuild_sv_chop_right_by_sv"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr(str1)), CBuildSVArg(sv32));
	// Some more chops for some edge-cases
	// Delimiter is part of long sequence of same chars
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("--");
		cbuild_sv_t part  = cbuild_sv_chop_right_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("baz")), 0,
			"Multi-char delim cbuild_sv_chop_right_by_sv from long sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("baz")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("foo---bar-")), 0,
			"Multi-char delim cbuild_sv_chop_right_by_sv from long sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("foo---bar-")),
			CBuildSVArg(sv));
	}
	// Delimiter is a single character
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("-");
		cbuild_sv_t part  = cbuild_sv_chop_right_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("baz")), 0,
			"Single-char delim cbuild_sv_chop_right_by_sv from long sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("baz")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("foo---bar--")), 0,
			"Single-char delim cbuild_sv_chop_right_by_sv from long sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("foo---bar--")),
			CBuildSVArg(sv));
	}
	// Extreme cases - start of a string
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("foo");
		cbuild_sv_t part  = cbuild_sv_chop_right_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("---bar---baz")), 0,
			"Delim of cbuild_sv_chop_right_by_sv is at the start of sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("---bar---baz")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("")), 0,
			"Delim of cbuild_sv_chop_right_by_sv is at the start of sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("")),
			CBuildSVArg(sv));
	}
	// Extreme cases - end of a string
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("baz");
		cbuild_sv_t part  = cbuild_sv_chop_right_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("")), 0,
			"Delim of cbuild_sv_chop_right_by_sv is at the end of sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("foo---bar---")), 0,
			"Delim of cbuild_sv_chop_right_by_sv is at the end of sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("for---bar---")),
			CBuildSVArg(sv));
	}
	// Extreme cases - full string
	{
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr(str);
		cbuild_sv_t part  = cbuild_sv_chop_right_by_sv(&sv, delim);
		TEST_ASSERT_EQ(cbuild_sv_cmp(part, cbuild_sv_from_cstr("")), 0,
			"Delim of cbuild_sv_chop_right_by_sv is full sequence: chopped part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(part));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("")), 0,
			"Delim of cbuild_sv_chop_right_by_sv is full sequence: remaining part mismatch"
			TEST_EXPECT_RMSG(CBuildSVFmt),
			CBuildSVArg(cbuild_sv_from_cstr("")),
			CBuildSVArg(sv));
	}
	return 0;
}
