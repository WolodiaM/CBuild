int main(void) {
	cbuild_allocator_t a = cbuild_allocator_from_libc();
	int* i1 = cbuild_ll_append(&a, NULL, 1);
	int* i2 = cbuild_ll_append(&a, i1, 2); CBUILD_UNUSED(i2);
	int* i3 = cbuild_ll_append(&a, i1, 3); CBUILD_UNUSED(i3);
	int* i4 = cbuild_ll_append(&a, i1, 4); CBUILD_UNUSED(i4);
	int* i5 = cbuild_ll_append(&a, i1, 5); CBUILD_UNUSED(i5);
	int* i6 = cbuild_ll_append(&a, i5, 6); CBUILD_UNUSED(i6);
	int* i7 = cbuild_ll_append(&a, i1, 7); CBUILD_UNUSED(i7);
	int* i8 = cbuild_ll_append(&a, i4, 8); CBUILD_UNUSED(i8);
	int* i9 = cbuild_ll_append(&a, i1, 9); CBUILD_UNUSED(i9);
	int* head = cbuild_ll_head(i7);
	TEST_ASSERT_EQ(
		head, i1,
		"Wrong head found for linked list"TEST_EXPECT_MSG(p),
		i1, head);
	TEST_ASSERT(cbuild_ll_prev(head) == NULL,
		"Head has non-null 'next'.");
	int* tail = cbuild_ll_tail(i2);
	TEST_ASSERT(cbuild_ll_next(tail) == NULL,
		"Tail has non-null 'next'.");
	TEST_ASSERT_EQ(
		tail, i9,
		"Wrong tail found for linked list"TEST_EXPECT_MSG(p),
		i9, tail);
	int* li = i1;
	for (int i = 1; i < 10; i++) {
		TEST_ASSERT_EQ(
			*li, i,
			"Wrong value at index %d"TEST_EXPECT_MSG(d),
			i - 1, i, *li);
		li = cbuild_ll_next(li);
	}
	cbuild_ll_free(&a, i9);
}
