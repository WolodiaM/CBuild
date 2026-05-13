int main(void) {
	cbuild_allocator_t a = cbuild_allocator_from_libc();
	int* i1 = cbuild_ll_append(&a, NULL, 1);
	int* i2 = cbuild_ll_append(&a, i1, 2);
	TEST_ASSERT_EQ(
		*i1, 1,
		"Wrong value for first node"TEST_EXPECT_MSG(d),
		1, *i1);
	TEST_ASSERT_EQ(
		*i2, 2,
		"Wrong value for second node"TEST_EXPECT_MSG(d),
		2, *i2);
	int* i1_n = cbuild_ll_next(i1);
	TEST_ASSERT_EQ(
		i1_n, i2,
		"Wrong 'next' value in first node"TEST_EXPECT_MSG(p),
		i2, i1_n);
	TEST_ASSERT(cbuild_ll_prev(i1) == NULL,
		"Non-null 'prev' value in first node.");
	int* i2_p = cbuild_ll_prev(i2);
	TEST_ASSERT_EQ(
		i2_p, i1,
		"Wrong 'prev' value in second node"TEST_EXPECT_MSG(p),
		i1, i2_p);
	TEST_ASSERT(cbuild_ll_next(i2) == NULL,
		"Non-null 'next' value in second node.");
	cbuild_ll_free(&a, i1);
}
