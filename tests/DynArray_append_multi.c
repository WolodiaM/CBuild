typedef struct da_int_t {
	int *data;
	size_t size;
	size_t capacity;
} da_int_t;
int main(void) {
	da_int_t da = {0};
	cbuild_da_append_many(&da, 1, 2, 3);
	TEST_ASSERT_EQ(
		da.size, 3,
		"Wrong element count after insertion"TEST_EXPECT_MSG(zu),
		(size_t)3, da.size);
	TEST_ASSERT_EQ(da.data[0], 1,
		"Wrong element at index 0"TEST_EXPECT_MSG(d), 1, da.data[0]);
	TEST_ASSERT_EQ(da.data[1], 2,
		"Wrong element at index 1"TEST_EXPECT_MSG(d), 2, da.data[1]);
	TEST_ASSERT_EQ(da.data[2], 3,
		"Wrong element at index 2"TEST_EXPECT_MSG(d), 3, da.data[2]);
	cbuild_da_clear(&da);
	return 0;
}
