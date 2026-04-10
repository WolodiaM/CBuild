int main(void) {
	cbuild_arena_t arena = {0};
	const size_t capacity = 1024;
	cbuild_arena_base_malloc(&arena, capacity);
	TEST_ASSERT_NEQ(arena.base, NULL, "Arena base allocation failed.");
	TEST_ASSERT_EQ(arena.capacity, capacity,
		"Arena capacity mismatch"TEST_EXPECT_MSG(zu), capacity, arena.capacity);
	TEST_ASSERT_EQ(arena.pointer, 0,
		"Invalid initial arena pointer"TEST_EXPECT_MSG(zu), (size_t)0, arena.pointer);
	void* p1 = cbuild_arena_malloc(&arena, 10);
	TEST_ASSERT_NEQ(p1, NULL, "First allocation failed.");
	TEST_ASSERT_EQ((uintptr_t)p1, (uintptr_t)arena.base,
		"First allocation should be at base.");
	size_t expected_ptr = (10 | (2 * sizeof(void*) - 1)) + 1;
	TEST_ASSERT_EQ(arena.pointer, expected_ptr,
		"Arena pointer after first allocation mismatch"TEST_EXPECT_MSG(zu),
		expected_ptr, arena.pointer);
	void* p2 = cbuild_arena_malloc(&arena, 20);
	TEST_ASSERT_NEQ(p2, NULL, "Second allocation failed");
	TEST_ASSERT_EQ((uintptr_t)p2, (uintptr_t)arena.base + expected_ptr,
		"Second allocation should follow first"TEST_EXPECT_MSG(p),
		(uintptr_t)arena.base + expected_ptr, (uintptr_t)p2);
	size_t expected_ptr2 = expected_ptr + ((20 | (2 * sizeof(void*) - 1)) + 1);
	TEST_ASSERT_EQ(arena.pointer, expected_ptr2,
		"Arena pointer after second allocation mismatch"TEST_EXPECT_MSG(zu),
		expected_ptr2, arena.pointer);
	// Test OOM
	void* p_large = cbuild_arena_malloc(&arena, capacity);
	TEST_ASSERT_EQ(p_large, NULL, "Large allocation should fail (OOM).");
	cbuild_arena_base_free(&arena);
	TEST_ASSERT_EQ(arena.base, NULL, "Arena base should be NULL after free.");
	TEST_ASSERT_EQ(arena.capacity, 0,
		"Invalid arena capacity after base free"TEST_EXPECT_MSG(zu),
		(size_t)0, arena.capacity);
	TEST_ASSERT_EQ(arena.pointer, 0,
		"Invalid arena pointer after base free"TEST_EXPECT_MSG(p),
		NULL, arena.pointer);
	return 0;
}
