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
int	main(void) {
	cbuild_map_t map = {0};
	map.elem_size = sizeof(char*) + sizeof(intptr_t);
	map.key_size = 0;
	char* key;
	void* elem;
	cbuild_map_init(&map, 256);
	key = "abc";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
	key = "def";
	elem = cbuild_map_get_or_alloc_ptr(&map, &key);
	((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 4;
	bool found_abc = false;
	bool found_def = false;
	cbuild_map_foreach_raw (&map, iter) {
		if(strcmp(((char**)iter)[0], "abc") == 0) found_abc = true;
		if(strcmp(((char**)iter)[0], "def") == 0) found_def = true;
	}
	TEST_ASSERT(found_abc, "Element with key \"abc\" was not iterated.");
	TEST_ASSERT(found_def, "Element with key \"def\" was not iterated.");
	cbuild_map_clear_ex(&map, test_cstr_free);
	return 0;
}
