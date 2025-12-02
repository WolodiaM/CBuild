size_t test_cstr_hash(const void* map, const void* key) {
	CBUILD_UNUSED(map);
	char* k = *((char**)key);
	return (size_t)(*k);
}
void test_cstr_free(const void* map, void* elem) {
	CBUILD_UNUSED(map);
	free(*((char**)elem));
}
char* test_strdup(char* str) {
	size_t len = strlen(str);
	char* ret = malloc(len + 1);
	memcpy(ret, str, len);
	ret[len] = '\0';
	return ret;
}
int main(void) {
	cbuild_map_t map = {0};
	map.elem_size = sizeof(char*) + sizeof(intptr_t);
	map.key_size = 0;
	char* key;
	void* elem;
	cbuild_map_init(&map, 256);
	key = "abc";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup("abc"); ((intptr_t*)elem)[1] = 1;
	key = "def";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
	key = "aaa";
	cbuild_map_remove_ex_ptr(&map, &key, test_cstr_free);
	key = "def";
	cbuild_map_remove_ex_ptr(&map, &key, test_cstr_free);
	key = "abc";
	TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 1,
		"Value for key 'abc' should still exist after other keys were removed"
		TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)1,
		((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
	key =	"def";
	TEST_ASSERT_EQ(cbuild_map_get_ptr(&map, &key), NULL,
		"Value for key 'def' should have been removed" TEST_EXPECT_MSG(p), NULL,
		(void*)cbuild_map_get_ptr(&map, &key));
	cbuild_map_clear_ex(&map, test_cstr_free);
}
