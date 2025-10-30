int main(void) {
	const char* str = "Hello, world!";
	cbuild_sv_t sv1 = cbuild_sv_from_parts(str, strlen(str));
	cbuild_sv_t sv2 = cbuild_sv_from_cstr(str);
	cbuild_sv_t sv3 = cbuild_sv_from_lit("ABC");
	TEST_ASSERT_EQ(sv1.data, str,
		"Wrong base pointer for cbuild_sv_from_parts"
		TEST_EXPECT_MSG(p), str, sv1.data);
	TEST_ASSERT_EQ(sv1.size, strlen(str),
		"Wrong lengths for cbuild_sv_from_parts"TEST_EXPECT_MSG(zu),
		strlen(str), sv1.size);
	TEST_ASSERT_EQ(sv2.data, str,
		"Wrong base pointer for cbuild_sv_from_cstr"
		TEST_EXPECT_MSG(p), str, sv2.data);
	TEST_ASSERT_EQ(sv2.size, strlen(str),
		"Wrong lengths for cbuild_sv_from_cstr"TEST_EXPECT_MSG(zu),
		strlen(str), sv2.size);
	TEST_ASSERT_MEMEQ(sv3.data, "ABC", 3,
		"Wrong value sv after cbuild_sv_from_lit"
		TEST_EXPECT_MSG(p), "ABC", sv2.data);
	TEST_ASSERT_EQ(sv3.size, 3,
		"Wrong lengths for cbuild_sv_from_lit"TEST_EXPECT_MSG(zu),
		3, sv2.size);
	return 0;
}
