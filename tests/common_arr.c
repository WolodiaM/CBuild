int main(void) {
	int arr[3] = {1, 2, 3};
	TEST_ASSERT_EQ(cbuild_arr_len(arr), 3,
		"cbuild_arr_len: Incorrect array length calculated"TEST_EXPECT_MSG(zu),
		(size_t)3, cbuild_arr_len(arr));
	TEST_ASSERT_EQ(cbuild_arr_get(arr, 1), 2,
		"cbuild_arr_get: Wrong element returned for index 1"TEST_EXPECT_MSG(d),
		2, cbuild_arr_get(arr, 1));
	return 0;
}
