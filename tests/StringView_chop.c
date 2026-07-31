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
#define TCASE_SV_CHOP(prefix, base, chopped, left, chopfn, ...)        \
	do {                                                                 \
		cbuild_sv_t sv = cbuild_sv_from_lit(base);                         \
		cbuild_sv_t res = chopfn(&sv __VA_OPT__(,) __VA_ARGS__);           \
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_lit(left)), 0,     \
			prefix ": remaining part mismatch"TEST_EXPECT_RMSG(CBuildSVFmt), \
			CBuildSVArg(cbuild_sv_from_lit(left)), CBuildSVArg(sv));         \
		TEST_ASSERT_EQ(cbuild_sv_cmp(res, cbuild_sv_from_lit(chopped)), 0, \
			prefix ": chopped part mismatch"TEST_EXPECT_RMSG(CBuildSVFmt),   \
			CBuildSVArg(cbuild_sv_from_lit(chopped)), CBuildSVArg(res));     \
	} while(0)
int main(void) {
	// cbuild_sv_chop
	TCASE_SV_CHOP("cbuild_sv_chop", "abc|def|-|ghj", "abc","|def|-|ghj", 
		cbuild_sv_chop, 3);
	TCASE_SV_CHOP("cbuild_sv_chop overflow", "abc|def|-|ghj", "abc|def|-|ghj", "", 
		cbuild_sv_chop, 40);
	// cbuild_sv_chop_by_delim
	TCASE_SV_CHOP("cbuild_sv_chop_by_delim", "abc|def|-|ghj", "abc", "def|-|ghj",
		cbuild_sv_chop_by_delim, '|');
	TCASE_SV_CHOP("cbuild_sv_chop_by_delim nomatch", "abc|def|-|ghj", "abc|def|-|ghj", "",
		cbuild_sv_chop_by_delim, '/');
	// cbuild_sv_chop_by_func
	TCASE_SV_CHOP("cbuild_sv_chop_by_func", "abc|def|-|ghj", "abc|def", "-|ghj",
		cbuild_sv_chop_by_func, tdelim1, NULL);
	TCASE_SV_CHOP("cbuild_sv_chop_by_func nomatch", "abc|def|-|ghj", "abc|def|-|ghj", "",
		cbuild_sv_chop_by_func, tdelim2, NULL);
	// cbuild_sv_chop_by_sv
	TCASE_SV_CHOP("cbuild_sv_chop_by_sv", "abc|def|-|ghj", "abc|def", "ghj",
		cbuild_sv_chop_by_sv, cbuild_sv_from_lit("|-|"));
	TCASE_SV_CHOP("cbuild_sv_chop_by_sv nomatch", "abc|def|-|ghj", "abc|def|-|ghj", "",
		cbuild_sv_chop_by_sv, cbuild_sv_from_lit("---"));
	TCASE_SV_CHOP("cbuild_sv_chop_by_sv delim is a part of long sequence",
		"foo---bar---baz", "foo", "-bar---baz",
		cbuild_sv_chop_by_sv, cbuild_sv_from_lit("--"));
	TCASE_SV_CHOP("cbuild_sv_chop_by_sv delim is a single character",
		"foo---bar---baz", "foo", "--bar---baz",
		cbuild_sv_chop_by_sv, cbuild_sv_from_lit("-"));
	TCASE_SV_CHOP("cbuild_sv_chop_by_sv delim is at the start of a string",
		"foo---bar---baz", "", "---bar---baz",
		cbuild_sv_chop_by_sv, cbuild_sv_from_lit("foo"));
	TCASE_SV_CHOP("cbuild_sv_chop_by_sv delim is at the end of a string",
		"foo---bar---baz", "foo---bar---", "",
		cbuild_sv_chop_by_sv, cbuild_sv_from_lit("baz"));
	TCASE_SV_CHOP("cbuild_sv_chop_by_sv delim is a full string",
		"foo---bar---baz", "", "",
		cbuild_sv_chop_by_sv, cbuild_sv_from_lit("foo---bar---baz"));
	// cbuild_sv_chop_by_space
	TCASE_SV_CHOP("cbuild_sv_chop_by_space with space", "abc def\tghj", "abc", "def\tghj",
		cbuild_sv_chop_by_space);
	TCASE_SV_CHOP("cbuild_sv_chop_by_space with tab", "abc\tdef ghj", "abc", "def ghj",
		cbuild_sv_chop_by_space);
	// cbuild_sv_chop_by_delim_any
	TCASE_SV_CHOP("cbuild_sv_chop_by_delim_any first delim", "abc|def|-|ghj", "abc", "def|-|ghj",
		cbuild_sv_chop_by_delim_any, "|-");
	TCASE_SV_CHOP("cbuild_sv_chop_by_delim_any second delim", "abc|def|-|ghj", "abc|def|", "|ghj",
		cbuild_sv_chop_by_delim_any, "/-");
	TCASE_SV_CHOP("cbuild_sv_chop_by_delim_any no delim", "abc|def|-|ghj", "abc|def|-|ghj", "",
		cbuild_sv_chop_by_delim_any, "/\\");
	// cbuild_sv_chop_right
	TCASE_SV_CHOP("cbuild_sv_chop_right", "abc|def|-|ghj", "ghj","abc|def|-|", 
		cbuild_sv_chop_right, 3);
	TCASE_SV_CHOP("cbuild_sv_chop_right overflow", "abc|def|-|ghj", "abc|def|-|ghj", "", 
		cbuild_sv_chop_right, 40);
	// cbuild_sv_chop_right_by_delim
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_delim", "abc|def|-|ghj", "ghj", "abc|def|-",
		cbuild_sv_chop_right_by_delim, '|');
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_delim nomatch", "abc|def|-|ghj", "abc|def|-|ghj", "",
		cbuild_sv_chop_right_by_delim, '/');
	// cbuild_sv_chop_by_func
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_func", "abc|def|-|ghj", "-|ghj", "abc|def",
		cbuild_sv_chop_right_by_func, tdelim3, NULL);
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_func nomatch", "abc|def|-|ghj", "abc|def|-|ghj", "",
		cbuild_sv_chop_right_by_func, tdelim2, NULL);
	// cbuild_sv_chop_right_by_sv
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_sv", "abc|def|-|ghj", "ghj", "abc|def",
		cbuild_sv_chop_right_by_sv, cbuild_sv_from_lit("|-|"));
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_sv nomatch", "abc|def|-|ghj", "abc|def|-|ghj", "",
		cbuild_sv_chop_right_by_sv, cbuild_sv_from_lit("---"));
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_sv delim is a part of long sequence",
		"foo---bar---baz", "baz", "foo---bar-",
		cbuild_sv_chop_right_by_sv, cbuild_sv_from_lit("--"));
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_sv delim is a single character",
		"foo---bar---baz", "baz", "foo---bar--",
		cbuild_sv_chop_right_by_sv, cbuild_sv_from_lit("-"));
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_sv delim is at the start of a string",
		"foo---bar---baz", "---bar---baz", "",
		cbuild_sv_chop_right_by_sv, cbuild_sv_from_lit("foo"));
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_sv delim is at the end of a string",
		"foo---bar---baz", "", "foo---bar---",
		cbuild_sv_chop_right_by_sv, cbuild_sv_from_lit("baz"));
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_sv delim is a full string",
		"foo---bar---baz", "", "",
		cbuild_sv_chop_right_by_sv, cbuild_sv_from_lit("foo---bar---baz"));
	// cbuild_sv_chop_right_by_space
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_space with space", "abc\tdef ghj", "ghj", "abc\tdef",
		cbuild_sv_chop_right_by_space);
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_space with tab", "abc def\tghj", "ghj", "abc def",
		cbuild_sv_chop_right_by_space);
	// cbuild_sv_chop_right_by_delim_any
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_delim_any first delim", "abc|def|-|ghj", "ghj", "abc|def|-",
		cbuild_sv_chop_right_by_delim_any, "|-");
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_delim_any second delim", "abc|def|-|ghj", "|ghj", "abc|def|",
		cbuild_sv_chop_right_by_delim_any, "/-");
	TCASE_SV_CHOP("cbuild_sv_chop_right_by_delim_any no delim", "abc|def|-|ghj", "abc|def|-|ghj", "",
		cbuild_sv_chop_right_by_delim_any, "/\\");
	return 0;
}
