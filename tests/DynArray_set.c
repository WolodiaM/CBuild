typedef struct da_int_t {
	int *data;
	size_t size;
	size_t capacity;
} da_int_t;
int main(void) {
	da_int_t da = {0};
	cbuild_da_resize(&da, 10);
	da.size = 2;
	cbuild_da_set(&da, 0, 1);
	TEST_ASSERT_EQ(da.data[0], 1,
		"Wrong element after set"TEST_EXPECT_MSG(d), 1, da.data[0]);
	cbuild_da_clear(&da);
	return 0;
}
