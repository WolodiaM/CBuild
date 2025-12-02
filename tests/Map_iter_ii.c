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
	elem = cbuild_map_get_or_alloc(&map, 3);
	elem[0] = 3; elem[1] = 4;
	bool found0 = false;
	bool found3 = false;
	cbuild_map_foreach (&map, int, iter) {
		if(iter[0] == 0) found0 = true;
		if(iter[0] == 3) found3 = true;
	}
	TEST_ASSERT(found0, "%s", "Element with key 0 was not iterated.");
	TEST_ASSERT(found3, "%s", "Element with key 3 was not iterated.");
	cbuild_map_clear(&map);
	return 0;
}
