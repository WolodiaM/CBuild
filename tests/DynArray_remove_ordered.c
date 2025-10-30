typedef struct da_int_t {
	int *data;
	size_t size;
	size_t capacity;
} da_int_t;
int main(void) {
	da_int_t da = {0};
	cbuild_da_append_many(&da, 1, 2, 3, 4);
	cbuild_da_remove(&da, 1);
	TEST_ASSERT_EQ(da.data[0], 1,
		"Wrong element at index 0"TEST_EXPECT_MSG(d), 1, da.data[0]);
	TEST_ASSERT_EQ(da.data[1], 3,
		"Wrong element at index 1"TEST_EXPECT_MSG(d), 3, da.data[1]);
	TEST_ASSERT_EQ(da.data[2], 4,
		"Wrong element at index 2"TEST_EXPECT_MSG(d), 4, da.data[2]);
	cbuild_da_clear(&da);
	return 0;
}
