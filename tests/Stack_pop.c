typedef struct stack_int_t {
	int* data;
	size_t ptr;
	size_t capacity;
} stack_int_t;
int main(void) {
	stack_int_t stack = {0};
	cbuild_stack_push(&stack, 1);
	cbuild_stack_push(&stack, 2);
	int val;
	val = cbuild_stack_pop(&stack);
	TEST_ASSERT_EQ(val, 2,
		"Wrong element read at index 1"TEST_EXPECT_MSG(d), 2, val);
	val = cbuild_stack_pop(&stack);
	TEST_ASSERT_EQ(val, 1,
		"Wrong element read at index 0"TEST_EXPECT_MSG(d), 1, val);
	cbuild_stack_clear(&stack);
	return 0;
}
