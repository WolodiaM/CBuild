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
	cbuild_map_init(&map, 256);
	elem = cbuild_map_get_or_alloc(&map, 0);
	elem[0] = 0; elem[1] = 1;
	elem = cbuild_map_get_or_alloc(&map, 1);
	elem[0] = 1; elem[1] = 2;
	cbuild_map_remove(&map, 1);
	cbuild_map_remove(&map, 2);
	TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 0))[1], 1,
		"Value for key 0 should still exist after other keys were removed"
		TEST_EXPECT_MSG(d), 1,
		((int*)cbuild_map_get(&map, 0))[1]);
	TEST_ASSERT_EQ(cbuild_map_get(&map, 1), NULL,
		"Value for key 1 should have been removed" TEST_EXPECT_MSG(p), NULL,
		(void*)cbuild_map_get(&map, 1));
	cbuild_map_clear(&map);
}
