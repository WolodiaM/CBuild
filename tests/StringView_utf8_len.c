int main(void) {
	// cbuild_sv_utf8cp_len
	cbuild_sv_t sv1 = cbuild_sv_from_cstr("Aр");
	int cp1len = cbuild_sv_utf8cp_len(sv1);
	TEST_ASSERT_EQ(cp1len, 1,
		"Wrong length of ASCII char" TEST_EXPECT_MSG(d), 1, cp1len);
	cbuild_sv_t sv2 = cbuild_sv_from_cstr("фa");
	int cp2len = cbuild_sv_utf8cp_len(sv2);
	TEST_ASSERT_EQ(cp2len, 2,
		"Wrong length of Cyrillic character" TEST_EXPECT_MSG(d), 2, cp2len);
	cbuild_sv_t sv3 = cbuild_sv_from_cstr("€a");
	int cp3len = cbuild_sv_utf8cp_len(sv3);
	TEST_ASSERT_EQ(cp3len, 3,
		"Wrong length of Euro currency symbol character"
		TEST_EXPECT_MSG(d), 3, cp3len);
	cbuild_sv_t sv4 = cbuild_sv_from_cstr("😀a");
	int cp4len = cbuild_sv_utf8cp_len(sv4);
	TEST_ASSERT_EQ(cp4len, 4,
		"Wrong length of Emoji character" TEST_EXPECT_MSG(d), 4, cp4len);
	// cbuild_sv_utf8len
	cbuild_sv_t sv5 = cbuild_sv_from_cstr("Привіт, world!€😀..."); // 19 chars
	size_t len1 = cbuild_sv_utf8len(sv5);
	TEST_ASSERT_EQ(len1, 19,
		"Wrong length computed for utf8 string"
		TEST_EXPECT_MSG(zu), (size_t)19, len1);
	size_t len2 = cbuild_sv_utf8len(cbuild_sv_from_lit(""));
	TEST_ASSERT_EQ(len2, 0,
		"Wrong length computed for utf8 string"
		TEST_EXPECT_MSG(zu), (size_t)0, len2);
	// cbuild_sv_utf8cp_right_len
	cbuild_sv_t sv6 = cbuild_sv_from_cstr("рA");
	int cp5len = cbuild_sv_utf8cp_right_len(sv6);
	TEST_ASSERT_EQ(cp5len, 1,
		"Wrong length of ASCII char" TEST_EXPECT_MSG(d), 1, cp5len);
	cbuild_sv_t sv7 = cbuild_sv_from_cstr("aф");
	int cp6len = cbuild_sv_utf8cp_right_len(sv7);
	TEST_ASSERT_EQ(cp6len, 2,
		"Wrong length of Cyrillic character" TEST_EXPECT_MSG(d), 2, cp6len);
	cbuild_sv_t sv8 = cbuild_sv_from_cstr("a€");
	int cp7len = cbuild_sv_utf8cp_right_len(sv8);
	TEST_ASSERT_EQ(cp7len, 3,
		"Wrong length of Euro currency symbol character"
		TEST_EXPECT_MSG(d), 3, cp7len);
	cbuild_sv_t sv9 = cbuild_sv_from_cstr("a😀");
	int cp8len = cbuild_sv_utf8cp_right_len(sv9);
	TEST_ASSERT_EQ(cp8len, 4,
		"Wrong length of Emoji character" TEST_EXPECT_MSG(d), 4, cp8len);
	return 0;
}
