size_t test_int_hash(const void* map, const void* key) {
	CBUILD_UNUSED(map);
	int k = *((int*)key);
	return (size_t)k;
}
int main(void) {
	cbuild_map_t map = {0};
	map.elem_size = 2 * sizeof(int);
	map.key_size = sizeof(int);
	int* elem;
	// Reads without hash collisions
	cbuild_map_init(&map, 256);
	elem = cbuild_map_get_or_alloc(&map, 0);
	elem[0] = 0; elem[1] = 1;
	elem = cbuild_map_get_or_alloc(&map, 1);
	elem[0] = 1; elem[1] = 2;
	TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 0))[1], 1,
		"[no collision] Wrong value read for key 0" TEST_EXPECT_MSG(d), 1,
		((int*)cbuild_map_get(&map, 0))[1]);
	TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 1))[1], 2,
		"[no collision] Wrong value read for key 1" TEST_EXPECT_MSG(d), 2,
		((int*)cbuild_map_get(&map, 1))[1]);
	cbuild_map_clear(&map);
	// Reads with hash collisions
	map.hash_func = test_int_hash;
	cbuild_map_init(&map, 1);
	elem = cbuild_map_get_or_alloc(&map, 0);
	elem[0] = 0; elem[1] = 1;
	elem = cbuild_map_get_or_alloc(&map, 1);
	elem[0] = 1; elem[1] = 2;
	TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 0))[1], 1,
		"[collision] Wrong value read for key 0" TEST_EXPECT_MSG(d), 1,
		((int*)cbuild_map_get(&map, 0))[1]);
	TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 1))[1], 2,
		"[collision] Wrong value read for key 1" TEST_EXPECT_MSG(d), 2,
		((int*)cbuild_map_get(&map, 1))[1]);
	cbuild_map_clear(&map);
	return 0;
}
