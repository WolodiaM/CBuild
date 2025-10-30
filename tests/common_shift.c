int main(void) {
	int  arr[3] = {1, 2, 3};
	int* arr_ptr = arr;
	int  arr_sz = 3;
	int  val  = cbuild_shift(arr_ptr, arr_sz);
	TEST_ASSERT_EQ(val, 1, "Wrong element shifted"TEST_EXPECT_MSG(d), 1, val);
	TEST_ASSERT_EQ(arr_sz, 2,
		"Wrong array length after shift"TEST_EXPECT_MSG(zu), 2, arr_sz);
	TEST_ASSERT_EQ((void*)arr_ptr, (void*)((char*)arr)+4,
		"Wrong array pointer"TEST_EXPECT_MSG(p),
		(void*)arr_ptr, (void*)(((char*)arr)+4));
	return 0;
}
