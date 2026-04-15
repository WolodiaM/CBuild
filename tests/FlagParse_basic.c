int main(int argc, char** argv) {
	cbuild_flag_set_option(CBUILD_FLAG_PASS_SEPARATOR);
	
	cbuild_flag_new("arg_req", .short_option = 'a', .num_arguments = 1,
		.group = "Required arguments", .argument_desc = "int",
		.desc = "Option with both long and short options.");
	cbuild_flag_new("list_req", .short_option = 'l', .num_arguments = -1,
		.group = "Required arguments", .argument_desc = "file",
		.desc = "Option with both long and short options.");
	cbuild_flag_new("tlist_req", .short_option = 't', .num_arguments = 3,
		.group = "Required arguments", .argument_desc = "boolean", .terminator = "^",
		.desc = "Option with both long and short options.");

	cbuild_flag_new("arg_opt", .short_option = 'A', .num_arguments = 1,
		.group = "Optional arguments", .argument_desc="float", .optional = true,
		.desc = "Option with both long and short options.");
	cbuild_flag_new("list_opt", .short_option = 'L', .num_arguments = -1,
		.group = "Optional arguments", .argument_desc="float", .optional = true,
		.desc = "Option with both long and short options.");
	cbuild_flag_new("tlist_opt", .short_option = 'T', .num_arguments = 4,
		.group = "Optional arguments", .argument_desc="float", .optional = true,
		.terminator = "^", .desc = "Option with both long and short options.");

	cbuild_flag_new("long", .desc = "Option with long option only", .repeat = true);

	cbuild_flag_new("a1", .short_option = 'b', .desc = "Some argument");
	cbuild_flag_new("a2", .short_option = 'c', .desc = "Another argument");
	
	cbuild_flag_parse(argc, argv);
	
	size_t arg_req_size = cbuild_flag_get_flag("arg_req")->size;
	TEST_ASSERT_EQ(arg_req_size, 1,
		"Error while parsing flag \"arg_req\" - wrong count"
		TEST_EXPECT_MSG(u), 1, arg_req_size);
	char* arg_req_data0 = cbuild_flag_get_flag("arg_req")->data[0];
	TEST_ASSERT_STREQ(arg_req_data0, "a",
		"Error while parsing flag \"arg_req\" - wrong argument 0"
		TEST_EXPECT_MSG(s), "a", arg_req_data0);
	TEST_ASSERT_EQ(cbuild_flag_get_flag("list_req"), NULL,
		"Error while parsing flag \"list_req\" - non-NULL value found");
	size_t tlist_req_size = cbuild_flag_get_flag("tlist_req")->size;
	TEST_ASSERT_EQ(tlist_req_size, 3,
		"Error while parsing flag \"tlist_req\" - wrong argument count"
		TEST_EXPECT_MSG(zu), tlist_req_size, 3);
	char* tlist_req_data0 = cbuild_flag_get_flag("tlist_req")->data[0];
	TEST_ASSERT_STREQ(tlist_req_data0, "-foo",
		"Error while parsing flag \"tlist_req\" - wrong argument 0"
		TEST_EXPECT_MSG(s), "-foo", tlist_req_data0);
	char* tlist_req_data1 = cbuild_flag_get_flag("tlist_req")->data[1];
	TEST_ASSERT_STREQ(tlist_req_data1, "---bar",
		"Error while parsing flag \"tlist_req\" - wrong argument 1"
		TEST_EXPECT_MSG(s), "---bar", tlist_req_data1);
	char* tlist_req_data2 = cbuild_flag_get_flag("tlist_req")->data[2];
	TEST_ASSERT_STREQ(tlist_req_data2, "baz",
		"Error while parsing flag \"tlist_req\" - wrong argument 2"
		TEST_EXPECT_MSG(s), "baz", tlist_req_data2);
	
	cbuild_arglist_t* arg_opt = cbuild_flag_get_flag("arg_opt");
	TEST_ASSERT_EQ(arg_opt, NULL,
		"Error while parsing flag \"arg_opt\" - flag should not be present"
		TEST_EXPECT_MSG(p), NULL, arg_opt);
	size_t list_opt_size = cbuild_flag_get_flag("list_opt")->size;
	TEST_ASSERT_EQ(list_opt_size, 2,
		"Error while parsing flag \"list_opt\" - wrong argument count",
		TEST_EXPECT_MSG(zu), 2, list_opt_size);
	char* list_opt_data0 = cbuild_flag_get_flag("list_opt")->data[0];
	TEST_ASSERT_STREQ(list_opt_data0, "foo",
		"Error while parsing flag \"list_opt\" - wrong argument 0"
		TEST_EXPECT_MSG(s), "foo", list_opt_data0);
	char* list_opt_data1 = cbuild_flag_get_flag("list_opt")->data[1];
	TEST_ASSERT_STREQ(list_opt_data1, "bar",
		"Error while parsing flag \"list_opt\" - wrong argument 1"
		TEST_EXPECT_MSG(s), "bar", list_opt_data1);
	cbuild_arglist_t* tlist_opt = cbuild_flag_get_flag("tlist_opt");
	TEST_ASSERT_EQ(tlist_opt, NULL,
		"Error while parsing flag \"tlist_opt\" - flag should not be present"
		TEST_EXPECT_MSG(p), NULL, tlist_opt);
	
	cbuild_arglist_t* noarg_long_flag = cbuild_flag_get_flag("long");
	TEST_ASSERT_NEQ(noarg_long_flag, NULL,
		"Error while parsing flag \"long\"",
		TEST_EXPECT_MSG(p), NULL, noarg_long_flag);

	size_t pargs_size = cbuild_flag_get_pargs()->size;
	TEST_ASSERT_EQ(pargs_size, 3,
		"Error while parsing positional arguments - wrong argument count"
		TEST_EXPECT_MSG(zu), 3, pargs_size);
	char* pargs_data0 = cbuild_flag_get_pargs()->data[0];
	TEST_ASSERT_STREQ(pargs_data0, "bar",
		"Error while parsing positional arguments - wrong argument 0"
		TEST_EXPECT_MSG(s), "bar", pargs_data0);
	char* pargs_data1 = cbuild_flag_get_pargs()->data[1];
	TEST_ASSERT_STREQ(pargs_data1, "--",
		"Error while parsing positional arguments - wrong argument 1"
		TEST_EXPECT_MSG(s), "--", pargs_data1);
	char* pargs_data2 = cbuild_flag_get_pargs()->data[2];
	TEST_ASSERT_STREQ(pargs_data2, "--foo",
		"Error while parsing positional arguments - wrong argument 2"
		TEST_EXPECT_MSG(s), "--foo", pargs_data2);
	
	cbuild_arglist_t* missing_flag1 = cbuild_flag_get_flag("a1");
	TEST_ASSERT_NEQ(missing_flag1, NULL,
		"Error while parsing flag \"a1\" - flag should be present"
		TEST_EXPECT_MSG(p), NULL, missing_flag1);
	cbuild_arglist_t* missing_flag2 = cbuild_flag_get_flag("a2");
	TEST_ASSERT_NEQ(missing_flag2, NULL,
		"Error while parsing flag \"a2\" - flag should be present"
		TEST_EXPECT_MSG(p), NULL, missing_flag2);
	return 0;
}
