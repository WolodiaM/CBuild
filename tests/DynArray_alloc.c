typedef struct da_int_t {
	int *data;
	size_t size;
	size_t capacity;
} da_int_t;
int main(void) {
	da_int_t da = {0};
	// Manual resize
	cbuild_da_resize(&da, 256);
	TEST_ASSERT_EQ(da.capacity, 256,
		"Wrong capacity after manual resize"TEST_EXPECT_MSG(zu),
		(size_t)256, da.capacity);
	TEST_ASSERT_NEQ(da.data, NULL, "Data pointer is NULL after manual resize.");
	cbuild_da_clear(&da);
	TEST_ASSERT_EQ(da.capacity, 0,
		"Wrong capacity after clear"TEST_EXPECT_MSG(zu),
		(size_t)0, da.capacity);
	TEST_ASSERT_EQ(da.data, NULL, "Data pointer is non-NULL after clear.");
	// Automatic resize
	cbuild_da_append(&da, 1);
	cbuild_da_append(&da, 1);
	cbuild_da_append(&da, 1);
	TEST_ASSERT_EQ(da.capacity, 4,
		"Wrong capacity after auto-resize on append"TEST_EXPECT_MSG(zu),
		(size_t)4, da.capacity);
	cbuild_da_clear(&da);
	return 0;
}
