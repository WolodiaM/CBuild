typedef struct pair_t {
	int key;
	int val;
	cbuild_map_tombstone_t tombstone;
} pair_t;
typedef struct map_t {
    pair_t* data;
    size_t size;
    size_t capacity;
    cbuild_map_hash_t hash;
    cbuild_map_keycmp_t keycmp;
} map_t;
size_t test_int_hash(const void* map, const void* key, size_t klen) {
	CBUILD_UNUSED(map);
	CBUILD_UNUSED(klen);
	int k = *((int*)key);
	return (size_t)k;
}
int main(void) {
	map_t map = {0};
	cbuild_map_init_num(&map);
	map.hash = test_int_hash;
	pair_t* elem;
	cbuild_map_resize(&map, 256);
	elem = cbuild_map_get(&map, 0);
	elem->val = 1;
	elem = cbuild_map_get(&map, 256);
	elem->val = 2;
	elem = cbuild_map_get(&map, 8);
	elem->val = 3;
	size_t idx = cbuild_map_find_idx(&map, 0);
	TEST_ASSERT_EQ(idx, 0,
		"Wrong index retrieved for key 0" TEST_EXPECT_MSG(zu),
		0, idx);
	size_t idx256 = (256 + __cbuild_map_step(256)) & (map.capacity - 1);
	idx = cbuild_map_find_idx(&map, 256);
	TEST_ASSERT_EQ(idx, idx256,
		"Wrong index retrieved for key 256" TEST_EXPECT_MSG(zu),
		idx256, idx);
	idx = cbuild_map_find_idx(&map, 8);
	TEST_ASSERT_EQ(idx, 8,
		"Wrong index retrieved for key 8" TEST_EXPECT_MSG(zu),
		8, idx);
	idx = cbuild_map_find_idx(&map, 11);
	TEST_ASSERT_EQ(idx, (size_t)-1,
		"Wrong index retrieved for key 11" TEST_EXPECT_MSG(zu),
		(size_t)-1, idx);
	cbuild_da_clear(&map);
	return 0;
}
