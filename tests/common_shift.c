int main(void) {
	int  arr[3] = {1, 2, 3};
	int* arr_ptr = arr;
	int  arr_sz = 3;
	int  val  = cbuild_shift(arr_ptr, arr_sz);
	TEST_ASSERT_EQ(val, 1, "cbuild_shift: Wrong element returned"
		TEST_EXPECT_MSG(d), 1, val);
	TEST_ASSERT_EQ(arr_sz, 2,
		"cbuild_shift: Wrong array size after shift"TEST_EXPECT_MSG(d), 2, arr_sz);
	TEST_ASSERT_EQ(arr_ptr, &arr[1],
		"cbuild_shift: Wrong array pointer after shift"TEST_EXPECT_MSG(p),
		&arr[1], arr_ptr);
	return 0;
}
