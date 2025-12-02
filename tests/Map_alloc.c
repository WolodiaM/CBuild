int main(void) {
	cbuild_map_t map = {0};
	map.elem_size = 2 * sizeof(int);
	map.key_size = sizeof(int);
	cbuild_map_init(&map, 256);
	TEST_ASSERT_EQ(
		map.nbuckets, 256,
		"Wrong bucket count was set after resize" TEST_EXPECT_MSG(zu),
		(size_t)256, map.nbuckets);
	TEST_ASSERT_NEQ(map.buckets, NULL, "%s",
		"Buckets array was not allocated after init.");
	cbuild_map_clear(&map);
	TEST_ASSERT_EQ(
		map.nbuckets, 0,
		"Wrong bucket count was set after clear" TEST_EXPECT_MSG(zu),
		(size_t)0, map.nbuckets);
	TEST_ASSERT_EQ(map.buckets, NULL,
		"Buckets array was not freed after clear.");
}
