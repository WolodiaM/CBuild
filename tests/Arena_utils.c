char* test_vsprintf(cbuild_arena_t* a, const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char* r = cbuild_arena_vsprintf(a, fmt, va);
	va_end(va);
	return r;
}
int main(void) {
	cbuild_arena_t arena = {0};
	cbuild_arena_base_malloc(&arena, 1024);
	// Test strdup
	const char* test_str = "Hello, world!";
	char* dup_str = cbuild_arena_strdup(&arena, test_str);
	TEST_ASSERT_NEQ(dup_str, NULL, "cbuild_arena_strdup failed.");
	TEST_ASSERT_STREQ(dup_str, test_str,
		"cbuild_arena_strdup: Wrong content"TEST_EXPECT_MSG(s), test_str, dup_str);
	TEST_ASSERT_NEQ(dup_str, test_str,
		"cbuild_arena_strdup: Should return different pointer.");
	// Test memdup
	const int test_mem[] = {1, 2, 3, 4, 5};
	int* dup_mem = cbuild_arena_memdup(&arena, test_mem, sizeof(test_mem));
	TEST_ASSERT_NEQ(dup_mem, NULL, "cbuild_arena_memdup failed.");
	TEST_ASSERT_MEMEQ(dup_mem, test_mem, sizeof(test_mem),
		"cbuild_arena_memdup: Wrong content.");
	TEST_ASSERT_NEQ((void*)dup_mem, (void*)test_mem,
		"cbuild_arena_memdup: Should return different pointer.");
	// Test vsprintf
	char* vfmt_str = test_vsprintf(&arena, "%s: %d", "val", 42);
	TEST_ASSERT_NEQ(vfmt_str, NULL, "cbuild_arena_vsprintf failed.");
	TEST_ASSERT_STREQ(vfmt_str, "val: 42",
		"cbuild_arena_vsprintf: Wrong content"TEST_EXPECT_MSG(s), "val: 42", vfmt_str);
	cbuild_arena_free(&arena);
	// Test OOM in utils
	cbuild_arena_base_realloc(&arena, 33); 
	cbuild_arena_free(&arena);
	char* large_dup = cbuild_arena_strdup(&arena, "This string is much longer than 33 bytes for sure");
	TEST_ASSERT_EQ(large_dup, NULL, "cbuild_arena_strdup should return NULL on OOM.");
	cbuild_arena_base_free(&arena);
	return 0;
}
