int main(void) {
	cbuild_sv_t sv = cbuild_sv_from_parts("abcdef", 3);
	const char* reference = "abc";
	char* cstr1 =	cbuild_sv_to_cstr(sv);
	TEST_ASSERT_STREQ(reference, cstr1, "Conversion to cstring failed"
		TEST_EXPECT_MSG(s), reference, cstr1);
	free(cstr1);
	void* temp_base = __cbuild_int_temp;
	char* cstr2 = cbuild_temp_sv_to_cstr(sv);
	TEST_ASSERT_STREQ(reference, cstr2, "Conversion to cstring failed"
		TEST_EXPECT_MSG(s), reference, cstr2);
	TEST_ASSERT_EQ(cstr2, temp_base,
		"Wrong pointer returned from a temp allocator" TEST_EXPECT_MSG(p),
		temp_base, cstr2);
	return 0;
}
