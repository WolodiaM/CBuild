const char* MSG = "Hello, world!";
int thread(void* context) {
	setvbuf(stdout, NULL, _IONBF, 0);
	sleep(2);
	printf("\tHello from child proc [%d]\n", getpid());
	fflush(stdout);
	memcpy(context, MSG, strlen(MSG) + 1);
	return 0;
}
int main(void) {
	cbuild_proc_ptr_t context = cbuild_proc_malloc(strlen(MSG) + 1);
	TEST_ASSERT_EQ(context.size, strlen(MSG) + 1,
		"Wrong amount of elements set in proc_ptr" TEST_EXPECT_MSG(zu),
		strlen(MSG) + 1, context.size);

	TEST_ASSERT_NEQ(context.ptr, NULL, "NULL returned from mmap!");
	cbuild_proc_t subproc = cbuild_proc_start(thread, context.ptr);
	printf("\tHello from main proc [%d]\n", getpid());
	fflush(stdout);
	int code = cbuild_proc_wait_code(subproc);
	TEST_ASSERT_EQ(code, 0, "Subprocess exited with non-0 code" TEST_EXPECT_MSG(d),
		0, code);
	TEST_ASSERT_STREQ(context.ptr, MSG,
		"Wrong shared memory data" TEST_EXPECT_MSG(s),
		MSG, (char*)context.ptr);
	cbuild_proc_free(context);
	return 0;
}
