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
	elem = cbuild_map_find(&map, 0);
	TEST_ASSERT_EQ(elem->val, 1,
		"Wrong value retrieved for key 0" TEST_EXPECT_MSG(d),
		1, elem->val);
	elem = cbuild_map_find(&map, 256);
	TEST_ASSERT_EQ(elem->val, 2,
		"Wrong value retrieved for key 256" TEST_EXPECT_MSG(d),
		2, elem->val);
	elem = cbuild_map_find(&map, 8);
	TEST_ASSERT_EQ(elem->val, 3,
		"Wrong value retrieved for key 8" TEST_EXPECT_MSG(d),
		3, elem->val);
	elem = cbuild_map_find(&map, 11);
	TEST_ASSERT_EQ(elem, NULL,
		"Wrong pair pointer retrieved for key 11" TEST_EXPECT_MSG(p),
		NULL, elem);
	cbuild_da_clear(&map);
	return 0;
}
