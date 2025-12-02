size_t test_cstr_hash(const void* map, const void* key) {
	CBUILD_UNUSED(map);
	char* k = *((char**)key);
	return (size_t)(*k);
}
char* test_strdup(char* str) {
	size_t len = strlen(str);
	char* ret = malloc(len + 1);
	memcpy(ret, str, len);
	ret[len] = '\0';
	return ret;
}
void test_cstr_free(const void* map, void* elem) {
	CBUILD_UNUSED(map);
	free(*((char**)elem));
}
int main(void) {
	cbuild_map_t map = {0};
	map.elem_size = sizeof(char*) + sizeof(intptr_t);
	map.key_size = 0;
	map.hash_func = test_cstr_hash;
	char* key;
	void* elem;
	// Writes without hash collisions
	cbuild_map_init(&map, 256);
	key = "abc";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
	key = "def";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
	TEST_ASSERT_NEQ(map.buckets['a'].vals, NULL,
		"[no collision] Key \"abc\" must be hashed in bucket %d but bucket is empty.", 'a');
	TEST_ASSERT_EQ(
		map.buckets['a'].nvals, 1,
		"[no collision] Wrong element count was set in bucket %d" TEST_EXPECT_MSG(zu),
		'a', (size_t)1, map.buckets['a'].nvals);
	TEST_ASSERT_EQ(((intptr_t*)map.buckets['a'].vals)[1], 1,
		"[no collision] Wrong element was written into bucket %d"
		TEST_EXPECT_RMSG("%"PRIu64),
		'a', (uint64_t)1, ((intptr_t*)map.buckets['a'].vals)[1]);
	TEST_ASSERT_NEQ(map.buckets['d'].vals, NULL,
		"[no collision] Key \"def\" must be hashed in bucket %d but bucket is empty.", 'd');
	TEST_ASSERT_EQ(map.buckets['d'].nvals, 1,
		"[no collision] Wrong element count was set in bucket %d" TEST_EXPECT_MSG(zu),
		'd', (size_t)1, map.buckets['d'].nvals);
	TEST_ASSERT_EQ(((intptr_t*)map.buckets['d'].vals)[1], 2,
		"[no collision] Wrong element was written into bucket %d"
		TEST_EXPECT_RMSG("%"PRIu64),
		'd', (uint64_t)2, ((intptr_t*)map.buckets['d'].vals)[1]);
	cbuild_map_clear_ex(&map, test_cstr_free);
	// Writes with hash collisions
	cbuild_map_init(&map, 256);
	key = "abc";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
	key = "aef";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
	TEST_ASSERT_NEQ(map.buckets['a'].vals, NULL,
		"[collision] Key 0 and 256 must be hashed in bucket %d but bucket is empty.", 'a');
	TEST_ASSERT_EQ(map.buckets['a'].nvals, 2,
		"[collision] Wrong element count was set in bucket %d" TEST_EXPECT_MSG(zu),
		'a', (size_t)2, map.buckets['a'].nvals);
	TEST_ASSERT_EQ(((intptr_t*)map.buckets['a'].vals)[1], 1,
		"[collision] Wrong element was written into bucket %d value 0"
		TEST_EXPECT_RMSG("%"PRIu64),
		'a', (uint64_t)1, ((intptr_t*)map.buckets['a'].vals)[1]);
	TEST_ASSERT_EQ(((intptr_t*)map.buckets['a'].vals)[3], 2,
		"[collision] Wrong element was written into bucket %d value 1"
		TEST_EXPECT_RMSG("%"PRIu64),
		'a', (uint64_t)2, ((intptr_t*)map.buckets['a'].vals)[3]);
	free(((char**)map.buckets['a'].vals)[0]);
	free(((char**)map.buckets['a'].vals)[2]);
	cbuild_map_clear(&map);
	// Writes with overwrite
	cbuild_map_init(&map, 256);
	key = "abc";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
	key = "abc";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	free(((char**)elem)[0]);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
	TEST_ASSERT_NEQ(map.buckets['a'].vals, NULL, "%s",
		"[overwrite] Value was not written at all.");
	TEST_ASSERT_EQ(map.buckets['a'].nvals, 1,
		"[overwrite] Wrong element count was set in bucket %d" TEST_EXPECT_MSG(zu),
		'a', (size_t)1, map.buckets[0].nvals);
	TEST_ASSERT_EQ(((intptr_t*)map.buckets['a'].vals)[1], 2,
		"[overwrite] Wrong element was written into bucket %d value 0"
		TEST_EXPECT_RMSG("%"PRIu64),
		'a', (uint64_t)2, ((intptr_t*)map.buckets[0].vals)[1]);
	free(((char**)map.buckets['a'].vals)[0]);
	cbuild_map_clear(&map);
	return 0;
}
