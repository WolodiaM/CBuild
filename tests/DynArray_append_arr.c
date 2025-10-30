typedef struct da_int_t {
	int *data;
	size_t size;
	size_t capacity;
} da_int_t;
int main(void) {
	da_int_t da = {0};
	int arr[] = { 1, 2, 3 };
	cbuild_da_append_arr(&da, arr, 2);
	TEST_ASSERT_EQ(da.size, 2,
		"Wrong element count after insertion" TEST_EXPECT_MSG(zu),
		(size_t)2, da.size);
	TEST_ASSERT_EQ(da.data[0], 1,
		"Wrong element at index 0"TEST_EXPECT_MSG(d), 1, da.data[0]);
	TEST_ASSERT_EQ(da.data[1], 2,
		"Wrong element at index 1"TEST_EXPECT_MSG(d), 2, da.data[1]);
	cbuild_da_clear(&da);
	return 0;
}
