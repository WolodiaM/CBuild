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
	// Writes without hash collisions
	cbuild_map_resize(&map, 256);
	elem = cbuild_map_get(&map, 0);
	elem->val = 1;
	elem = cbuild_map_get(&map, 1);
	elem->val = 2;
	TEST_ASSERT_EQ(map.data[0].val, 1,
		"[no collision] Wrong value was written into slot 0" TEST_EXPECT_MSG(d),
		1, map.data[0].val);
	TEST_ASSERT_EQ(map.data[0].tombstone, CBUILD_MAP_FULL,
		"[no collision] Wrong tombstone value was written into slot 0" TEST_EXPECT_MSG(d),
		CBUILD_MAP_FULL, map.data[0].tombstone);
	TEST_ASSERT_EQ(map.data[1].val, 2,
		"[no collision] Wrong value was written into slot 1" TEST_EXPECT_MSG(d),
		2, map.data[1].val);
	TEST_ASSERT_EQ(map.data[1].tombstone, CBUILD_MAP_FULL,
		"[no collision] Wrong tombstone value was written into slot 1" TEST_EXPECT_MSG(d),
		CBUILD_MAP_FULL, map.data[1].tombstone);
	cbuild_da_clear(&map);
	// Writes with hash collisions
	cbuild_map_resize(&map, 256);
	elem = cbuild_map_get(&map, 0);
	elem->val = 1;
	pair_t* elem2 = cbuild_map_get(&map, 256);
	elem2->val = 2;
	TEST_ASSERT_EQ(map.data[0].val, 1,
		"[collision] Wrong value was written into slot 0" TEST_EXPECT_MSG(d),
		1, map.data[0].val);
	TEST_ASSERT_EQ(map.data[0].tombstone, CBUILD_MAP_FULL,
		"[collision] Wrong tombstone value was written into slot 0" TEST_EXPECT_MSG(d),
		CBUILD_MAP_FULL, map.data[0].tombstone);
	size_t idx256 = (256 + __cbuild_map_step(256)) & (map.capacity - 1);
	TEST_ASSERT_EQ(map.data[idx256].val, 2,
		"[collision] Wrong value was written into slot %zu" TEST_EXPECT_MSG(d),
		idx256, 2, map.data[idx256].val);
	TEST_ASSERT_EQ(map.data[idx256].tombstone, CBUILD_MAP_FULL,
		"[collision] Wrong tombstone value was written into slot %zu" TEST_EXPECT_MSG(d),
		idx256, CBUILD_MAP_FULL, map.data[idx256].tombstone);
	cbuild_da_clear(&map);
	// Writes with overwrite
	cbuild_map_resize(&map, 256);
	elem = cbuild_map_get(&map, 0);
	elem->val = 1;
	elem = cbuild_map_get(&map, 0);
	elem->val = 2;
	TEST_ASSERT_EQ(map.data[0].val, 2,
		"[overwrite] Wrong value was written into slot 0" TEST_EXPECT_MSG(d),
		2, map.data[0].val);
	TEST_ASSERT_EQ(map.data[0].tombstone, CBUILD_MAP_FULL,
		"[overwrite] Wrong tombstone value was written into slot 0" TEST_EXPECT_MSG(d),
		CBUILD_MAP_FULL, map.data[0].tombstone);
	cbuild_da_clear(&map);
	return 0;
}
