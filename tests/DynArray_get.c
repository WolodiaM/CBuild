typedef struct da_int_t {
	int *data;
	size_t size;
	size_t capacity;
} da_int_t;
int main(void) {
	da_int_t da = {0};
	cbuild_da_append(&da, 1);
	cbuild_da_append(&da, 2);
	TEST_ASSERT_EQ(*cbuild_da_get(da, 0), 1,
		"Wrong element read at index 0"TEST_EXPECT_MSG(d),
		1, *cbuild_da_get(da, 0));
	TEST_ASSERT_EQ(*cbuild_da_get(da, 1), 2,
		"Wrong element read at index 1"TEST_EXPECT_MSG(d),
		2, *cbuild_da_get(da, 1));
	cbuild_da_clear(&da);
	return 0;
}
