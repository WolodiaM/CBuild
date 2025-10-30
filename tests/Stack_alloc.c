typedef struct stack_int_t {
	int* data;
	size_t ptr;
	size_t capacity;
} stack_int_t;
int main(void) {
	stack_int_t stack = {0};
	cbuild_stack_resize(&stack, 256);
	TEST_ASSERT_EQ(	stack.capacity, 256,
		"Wrong capacity was set on resize"TEST_EXPECT_MSG(zu),
		(size_t)256, stack.capacity);
	TEST_ASSERT_NEQ(stack.data, NULL, "Data pointer NULL after resize.");
	cbuild_stack_clear(&stack);
	TEST_ASSERT_EQ(stack.data, NULL, "Data pointer non-null after resize.");
	TEST_ASSERT_EQ(stack.capacity, 0,
		"Wrong capcity was set on clear"TEST_EXPECT_MSG(zu),
		(size_t)0, stack.capacity);
	// TODO: Automatic resize
	cbuild_stack_push(&stack, 1);
	cbuild_stack_push(&stack, 1);
	cbuild_stack_push(&stack, 1);
	TEST_ASSERT_EQ(stack.capacity, 4,
		"Wrong capacity after allocation"TEST_EXPECT_MSG(zu),
		(size_t)4, stack.capacity);
	cbuild_stack_clear(&stack);
	return 0;
}
