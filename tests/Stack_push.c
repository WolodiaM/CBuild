typedef struct stack_int_t {
	int* data;
	size_t ptr;
	size_t capacity;
} stack_int_t;
int main(void) {
	stack_int_t stack = {0};
	cbuild_stack_push(&stack, 1);
	cbuild_stack_push(&stack, 2);
	TEST_ASSERT_EQ(stack.ptr, 2,
		"Wrong element count after insertion"
		TEST_EXPECT_MSG(zu), (size_t)2, stack.ptr);
	TEST_ASSERT_EQ(stack.data[0], 1,
		"Wrong element at index 0"TEST_EXPECT_MSG(d), 1, stack.data[0]);
	TEST_ASSERT_EQ(stack.data[1], 2,
		"Wrong element at index 1"TEST_EXPECT_MSG(d), 2, stack.data[1]);
	cbuild_stack_clear(&stack);
	return 0;
}
