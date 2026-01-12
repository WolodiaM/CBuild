typedef struct da_int_t {
	int *data;
	size_t size;
	size_t capacity;
} da_int_t;
int main(void) {
	da_int_t da = {0};
	cbuild_da_append_many(&da, 1, 2, 3, 4);
	int found = 0;
	cbuild_da_foreach(da, elem) {
		TEST_ASSERT_EQ(*elem, found+1,
			"Wrong element at index %zu"TEST_EXPECT_MSG(d),
			elem - da.data, found + 1, *elem);
		found++;
	}
	cbuild_da_clear(&da);
	return 0;
}
