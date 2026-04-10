int main(void) {
	cbuild_arena_t arena = {0};
	cbuild_arena_base_malloc(&arena, 1024);
	// Allocation 1
	char* p1 = cbuild_arena_malloc(&arena, 32);
	TEST_ASSERT_NEQ(p1, NULL, "allocation failed.");
	memset(p1, 0xAB, 32);
	// Realloc to same size
	char* p2 = cbuild_arena_realloc(&arena, p1, 32);
	TEST_ASSERT_NEQ(p2, NULL, "realloc failed.");
	TEST_ASSERT_NEQ(p2, p1, "realloc should return new pointer.");
	TEST_ASSERT_MEMEQ(p2, p1, 32, "invalid data after realloc.");
	// Realloc to smaller size
	char* p3 = cbuild_arena_realloc(&arena, p2, 16);
	TEST_ASSERT_NEQ(p3, NULL, "shrinking realloc failed.");
	TEST_ASSERT_MEMEQ(p3, p2, 16, "invalid data after shrinking realloc.");
	cbuild_arena_base_free(&arena);
	return 0;
}
