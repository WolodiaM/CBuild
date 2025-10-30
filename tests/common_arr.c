int main(void) {
	int arr[3] = {1, 2, 3};
	TEST_ASSERT_EQ(cbuild_arr_len(arr), 3,
		"Incorect array length calculated"TEST_EXPECT_MSG(zu),
		cbuild_arr_len(arr), (size_t)3);
	TEST_ASSERT_EQ(cbuild_arr_get(arr, 1), 2,
		"Wrong element at array at index 1"TEST_EXPECT_MSG(d),
		cbuild_arr_get(arr, 1), 2);
	return 0;
}
