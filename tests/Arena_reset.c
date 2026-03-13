int main(void) {
	cbuild_arena_t arena = {0};
	const size_t capacity = 1024;
	cbuild_arena_base_alloc(&arena, capacity);
	cbuild_arena_malloc(&arena, 16);
	size_t checkpoint1 = cbuild_arena_checkpoint(&arena);
	TEST_ASSERT_EQ(checkpoint1, (16 | (2 * sizeof(void*) - 1)) + 1,
		"Arena checkpoint 1 mismatch"TEST_EXPECT_MSG(zu),
		(16 | (2 * sizeof(void*) - 1)) + 1, checkpoint1);
	void* p2 = cbuild_arena_malloc(&arena, 32);
	size_t checkpoint2 = cbuild_arena_checkpoint(&arena);
	TEST_ASSERT_NEQ(checkpoint1, checkpoint2,
		"Checkpoint should change after allocation.");
	cbuild_arena_reset(&arena, checkpoint1);
	TEST_ASSERT_EQ(arena.pointer, checkpoint1,
		"Arena pointer after reset mismatch"TEST_EXPECT_MSG(zu),
		checkpoint1, arena.pointer);
	void* p3 = cbuild_arena_malloc(&arena, 16);
	TEST_ASSERT_EQ(p3, p2,
		"Re-allocation after reset should use the same address"TEST_EXPECT_MSG(p),
		p2, p3);
	cbuild_arena_free(&arena);
	TEST_ASSERT_EQ(arena.pointer, 0,
		"Arena pointer after free mismatch"TEST_EXPECT_MSG(zu),
		(size_t)0, arena.pointer);
	TEST_ASSERT_EQ(arena.capacity, capacity,
		"Arena capacity should remain after cbuild_arena_free"TEST_EXPECT_MSG(zu),
		capacity, arena.capacity);
	cbuild_arena_base_free(&arena);
	return 0;
}
