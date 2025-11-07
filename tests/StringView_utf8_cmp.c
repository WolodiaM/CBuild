int main(void) {
	cbuild_sv_t sv1 = cbuild_sv_from_cstr("aф€😀");
	cbuild_sv_t sv2 = cbuild_sv_from_cstr("aф€");
	cbuild_sv_t sv3 = cbuild_sv_from_cstr("aaa");
	TEST_ASSERT_EQ(cbuild_sv_utf8cmp(sv1, sv1), 0,
		"Comparison of utf8 strings is broken, same strings are not reported as such!");
	TEST_ASSERT_EQ(cbuild_sv_utf8cmp(sv1, sv2), 2,
		"Comparison of utf8 strings is broken, different in size strings are not reported as such!");
	TEST_ASSERT_EQ(cbuild_sv_utf8cmp(sv2, sv3), 1,
		"Comparison of utf8 strings is broken, different in content strings are not reported as such!");
	return 0;
}
