int main(void) {
	// cbuild_sv_to_cstr
	cbuild_sv_t sv = cbuild_sv_from_parts("abcdef", 3);
	const char* reference = "abc";
	char* cstr1 =	cbuild_sv_to_cstr(sv);
	TEST_ASSERT_STREQ(reference, cstr1, "Conversion to c-string failed"
		TEST_EXPECT_MSG(s), reference, cstr1);
	free(cstr1);
	// CBuild_sv_to_temp_cstr
	cbuild_temp_malloc(0);
	cbuild_temp_reset(0);
	void* temp_base = __cbuild_temp_arena.base;
	char* cstr2 = cbuild_sv_to_temp_cstr(sv);
	TEST_ASSERT_STREQ(reference, cstr2, "Conversion to c-string failed"
		TEST_EXPECT_MSG(s), reference, cstr2);
	TEST_ASSERT_EQ(cstr2, temp_base,
		"Wrong pointer returned from a temp allocator" TEST_EXPECT_MSG(p),
		temp_base, cstr2);
	// cbuild_sv_to_arena_cstr
	cbuild_arena_t arena = {0};
	cbuild_arena_base_malloc(&arena, 1024);
	void* arena_base = arena.base;
	char* cstr3 = cbuild_sv_to_arena_cstr(&arena, sv);
	TEST_ASSERT_STREQ(reference, cstr3, "Conversion to c-string failed"
		TEST_EXPECT_MSG(s), reference, cstr3);
	TEST_ASSERT_EQ(cstr3, arena_base,
		"Wrong pointer returned from a arena allocator" TEST_EXPECT_MSG(p),
		arena_base, cstr3);
	cbuild_arena_base_free(&arena);
	return 0;
}
