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
	char* key;
	void* elem;
	// Reads without hash collisions
	cbuild_map_init(&map, 256);
	key = "abc";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
	key = "def";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
	key = "abc";
	TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 1,
		"[no collision] Wrong value read for key 'abc'"
		TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)1,
		((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
	key = "def";
	TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 2,
		"[no collision] Wrong value read for key 'def'"
		TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)2,
		((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
	cbuild_map_clear_ex(&map, test_cstr_free);
	// Reads with hash collisions
	map.hash_func = test_cstr_hash;
	cbuild_map_init(&map, 1);
	key = "abc";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
	key = "aef";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
	key = "abc";
	TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 1,
		"[collision] Wrong value read for key 'abc'"
		TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)1,
		((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
	key = "aef";
	TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 2,
		"[collision] Wrong value read for key 'aef'"
		TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)2,
		((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
	cbuild_map_clear_ex(&map, test_cstr_free);
	return 0;
}
