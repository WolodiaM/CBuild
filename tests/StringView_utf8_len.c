int main(void) {
	cbuild_sv_t sv1 = cbuild_sv_from_cstr("A");
	int cp1len = cbuild_sv_utf8cp_len(sv1);
	TEST_ASSERT_EQ(cp1len, 1,
		"Wrong length of ASCII char" TEST_EXPECT_MSG(d), 1, cp1len);
	cbuild_sv_t sv2 = cbuild_sv_from_cstr("ф");
	int cp2len = cbuild_sv_utf8cp_len(sv2);
	TEST_ASSERT_EQ(cp2len, 2,
		"Wrong length of Cyrillic character" TEST_EXPECT_MSG(d), 2, cp2len);
	cbuild_sv_t sv3 = cbuild_sv_from_cstr("€");
	int cp3len = cbuild_sv_utf8cp_len(sv3);
	TEST_ASSERT_EQ(cp3len, 3,
		"Wrong length of Euro currency symbol character"
		TEST_EXPECT_MSG(d), 3, cp3len);
	cbuild_sv_t sv4 = cbuild_sv_from_cstr("😀");
	int cp4len = cbuild_sv_utf8cp_len(sv4);
	TEST_ASSERT_EQ(cp4len, 4,
		"Wrong length of Emoji character" TEST_EXPECT_MSG(d), 4, cp4len);
	cbuild_sv_t sv5 = cbuild_sv_from_cstr("Привіт, world!€😀..."); // 19 chars
	size_t len1 = cbuild_sv_utf8len(sv5);
	TEST_ASSERT_EQ(len1, 19,
		"Wrong length computed for utf8 string"
		TEST_EXPECT_MSG(zu), (size_t)19, len1);
	size_t len2 = cbuild_sv_utf8len(cbuild_sv_from_lit(""));
	TEST_ASSERT_EQ(len2, 0,
		"Wrong length computed for utf8 string"
		TEST_EXPECT_MSG(zu), (size_t)0, len2);
	return 0;
}
