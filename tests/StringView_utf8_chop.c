bool tdelim3(const cbuild_sv_t* sv, void* arg) {
	cbuild_sv_t temp = *sv;
	uint32_t cp = cbuild_sv_chop_utf8(&temp);
	*(uint32_t*)arg = cp;
	return cp == 1092;
}
int main(void) {
	// cbuild_sv_chop_utf8
	cbuild_sv_t sv1 = cbuild_sv_from_cstr("aф€😀");
	uint32_t cp1 = cbuild_sv_chop_utf8(&sv1);
	TEST_ASSERT_EQ(cp1, 'a',
		"Wrong single-byte  character chopped from utf8 string"
		TEST_EXPECT_RMSG("0x%08X"), 'a', cp1);
	cp1 = cbuild_sv_chop_utf8(&sv1);
	TEST_ASSERT_EQ(cp1, 1092,
		"Wrong two-byte character chopped from utf8 string"
		TEST_EXPECT_RMSG("0x%08X"), 1092, cp1);
	cp1 = cbuild_sv_chop_utf8(&sv1);
	TEST_ASSERT_EQ(cp1, 8364,
		"Wrong three-byte character chopped from utf8 string"
		TEST_EXPECT_RMSG("0x%08X"), 8364, cp1);
	cp1 = cbuild_sv_chop_utf8(&sv1);
	TEST_ASSERT_EQ(cp1, 128512,
		"Wrong four-byte character chopped from utf8 string"
		TEST_EXPECT_RMSG("0x%08X"), 128512, cp1);
	// cbuild_sv_chop_by_utf8
	cbuild_sv_t sv2 = cbuild_sv_from_cstr("aaaфbbb");
	cbuild_sv_t chopped1 = cbuild_sv_chop_by_utf8(&sv2, 1092);
	TEST_ASSERT_EQ(chopped1.size, 3,
		"Wrong string view size after returned a chop_by_utf8"
		TEST_EXPECT_MSG(zu), (size_t)3, chopped1.size);
	TEST_ASSERT_EQ(sv2.size, 3, "Wrong string view size after a chop_by_utf8"
		TEST_EXPECT_MSG(zu), (size_t)3, sv2.size);
	TEST_ASSERT_MEMEQ(chopped1.data, "aaa", 3,
		"Wrong string view returned from chop_by_utf8" TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("aaa")), CBuildSVArg(chopped1));
	TEST_ASSERT_MEMEQ(sv2.data, "bbb", 3,
		"Wrong string view left after a chop_by_utf8" TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("bbb")), CBuildSVArg(sv2));
	// cbuild_chop_by_func_utf8
	cbuild_sv_t sv3 = cbuild_sv_from_cstr("aaaфbbb");
	uint32_t cp2 = 0;
	cbuild_sv_t chopped2 = cbuild_sv_chop_by_func_utf8(&sv3, tdelim3, &cp2);
	TEST_ASSERT_EQ(cp2, 1092,
		"Wrong character chosen as delimiter by chop_by_func_utf8"
		TEST_EXPECT_RMSG("0x%08X"), 1092, cp2);
	TEST_ASSERT_EQ(chopped2.size, 3,
		"Wrong string view size returned after a chop_by_func_utf8"
		TEST_EXPECT_MSG(zu), (size_t)3, chopped2.size);
	TEST_ASSERT_EQ(sv3.size, 3, "Wrong string view size after a chop_by_func_utf8"
		TEST_EXPECT_MSG(zu), (size_t)3, sv3.size);
	TEST_ASSERT_MEMEQ(chopped2.data, "aaa", 3,
		"Wrong string view returned by chop_by_func_utf8"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("aaa")), CBuildSVArg(chopped2));
	TEST_ASSERT_MEMEQ(sv3.data, "bbb", 3,
		"Wrong string view left after a chop_by_func_utf8"
		TEST_EXPECT_RMSG(CBuildSVFmt),
		CBuildSVArg(cbuild_sv_from_cstr("bbb")), CBuildSVArg(sv3));
	return 0;
}
