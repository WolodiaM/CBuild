typedef struct da_int_t {
	int* data;
	size_t size;
	size_t capacity;
} da_int_t;
int main(void) {
	da_int_t stack = {0};
	cbuild_da_append(&stack, 1);
	cbuild_da_append(&stack, 2);
	int val;
	val = cbuild_da_pop(&stack);
	TEST_ASSERT_EQ(val, 2,
		"Wrong element read at index 1"TEST_EXPECT_MSG(d), 2, val);
	val = cbuild_da_pop(&stack);
	TEST_ASSERT_EQ(val, 1,
		"Wrong element read at index 0"TEST_EXPECT_MSG(d), 1, val);
	cbuild_da_clear(&stack);
	return 0;
}
