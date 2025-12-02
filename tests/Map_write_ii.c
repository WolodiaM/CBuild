_Static_assert(sizeof(char*) == sizeof(intptr_t),
	"Cannot test cstr->int map on this platform");
size_t test_int_hash(const void* map, const void* key) {
	CBUILD_UNUSED(map);
	int k = *((int*)key);
	return (size_t)k;
}
int main(void) {
	cbuild_map_t map = {0};
	map.elem_size = 2 * sizeof(int);
	map.key_size = sizeof(int);
	map.hash_func = test_int_hash;
	int* elem;
	// Writes without hash collisions
	cbuild_map_init(&map, 256);
	elem = cbuild_map_get_or_alloc(&map, 0);
	elem[0] = 0; elem[1] = 1;
	elem = cbuild_map_get_or_alloc(&map, 1);
	elem[0] = 1; elem[1] = 2;
	TEST_ASSERT_NEQ(map.buckets[0].vals, NULL,
		"[no collision] Key 0 must be hashed in bucket 0 but bucket is empty.");
	TEST_ASSERT_EQ(map.buckets[0].nvals, 1,
		"[no collision] Wrong element count was set in bucket 0" TEST_EXPECT_MSG(zu),
		(size_t)1, map.buckets[0].nvals);
	TEST_ASSERT_EQ(((int*)map.buckets[0].vals)[1], 1,
		"[no collision] Wrong value was written into bucket 0" TEST_EXPECT_MSG(d),
		1, ((int*)map.buckets[0].vals)[1]);
	TEST_ASSERT_NEQ(map.buckets[1].vals, NULL,
		"[no collision] Key 1 must be hashed in bucket 1 but bucket is empty.");
	TEST_ASSERT_EQ(map.buckets[1].nvals, 1,
		"[no collision] Wrong element count was set in bucket 1" TEST_EXPECT_MSG(zu),
		(size_t)1, map.buckets[1].nvals);
	TEST_ASSERT_EQ(((int*)map.buckets[1].vals)[1], 2,
		"[no collision] Wrong value was written into bucket 1" TEST_EXPECT_MSG(d),
		2, ((int*)map.buckets[1].vals)[1]);
	cbuild_map_clear(&map);
	// Writes with hash collisions
	cbuild_map_init(&map, 256);
	elem = cbuild_map_get_or_alloc(&map, 0);
	elem[0] = 0; elem[1] = 1;
	elem = cbuild_map_get_or_alloc(&map, 256);
	elem[0] = 256; elem[1] = 2;
	TEST_ASSERT_NEQ(map.buckets[0].vals, NULL,
		"[collision] Keys 0 and 256 must be hashed in bucket 0 but bucket is empty.");
	TEST_ASSERT_EQ(map.buckets[0].nvals, 2,
		"[collision] Wrong element count was set in bucket 0" TEST_EXPECT_MSG(zu),
		(size_t)2, map.buckets[0].nvals);
	TEST_ASSERT_EQ(((int*)map.buckets[0].vals)[1], 1,
		"[collision] Wrong value was written for key 0"
		TEST_EXPECT_MSG(d), 1, ((int*)map.buckets[0].vals)[1]);
	TEST_ASSERT_EQ(((int*)map.buckets[0].vals)[3], 2,
		"[collision] Wrong value was written for key 256"
		TEST_EXPECT_MSG(d), 2, ((int*)map.buckets[0].vals)[3]);
	cbuild_map_clear(&map);
	// Writes with overwrite
	cbuild_map_init(&map, 256);
	elem = cbuild_map_get_or_alloc(&map, 0);
	elem[0] = 0; elem[1] = 1;
	elem = cbuild_map_get_or_alloc(&map, 0);
	elem[0] = 0; elem[1] = 2;
	TEST_ASSERT_NEQ(map.buckets[0].vals, NULL,
		"[overwrite] Bucket is empty after writing a value.");
	TEST_ASSERT_EQ(map.buckets[0].nvals, 1,
		"[overwrite] Wrong element count was set in bucket 0" TEST_EXPECT_MSG(zu),
		(size_t)1, map.buckets[0].nvals);
	TEST_ASSERT_EQ(((int*)map.buckets[0].vals)[1], 2,
		"[overwrite] Wrong value was written for key 0"
		TEST_EXPECT_MSG(d), 2, ((int*)map.buckets[0].vals)[1]);
	cbuild_map_clear(&map);
	return 0;
}
