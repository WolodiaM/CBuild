int main(int argc, char** argv) {
	cbuild_flag_new("-separator");
	
	cbuild_flag_new("arg_req\ta\targ=arg;thint=int;group=Requires arguments\tOption with both long and short options.");
	cbuild_flag_new("list_req\tl\targ=list;thint=file;group=Requires arguments\tOption with both long and short options.");
	cbuild_flag_new("tlist_req\tt\targ=tlist;tdelim=^;thint=boolean;len=3;group=Requires arguments\tOption with both long and short options.");
	
	cbuild_flag_new("arg_opt\tA\targ=arg?;thint=float;group=Argument is optional\tOption with both long and short options.");
	cbuild_flag_new("list_opt\tL\targ=list?;thint=float;group=Argument is optional\tOption with both long and short options.");
	cbuild_flag_new("tlist_opt\tT\targ=tlist?;thint=file;tdelim=^;len=4;group=Argument is optional\tOption with both long and short options.");
	
	cbuild_flag_new("long\n\tOption with long option only.");
	
	cbuild_flag_new("a1\tb\t\tSome argument");
	cbuild_flag_new("a2\tc\t\tAnother argument");
	
	cbuild_flag_new("-alias:arg_req:arg1,arg2");
	
	cbuild_flag_new("-group:Requires arguments:All flags from this group requires at least one argument.");
	cbuild_flag_new("-group:Argument is optional:Flags from this group can live even without any arguments!");
	
	cbuild_flag_parse(argc, argv);
	
	size_t arg_req_size = cbuild_flag_get_flag("arg_req")->size;
	TEST_ASSERT_EQ(arg_req_size, 1,
		"Error while parsing flag \"arg_req\" - wrong count"
		TEST_EXPECT_MSG(u), 1, arg_req_size);
	char* arg_req_data0 = cbuild_flag_get_flag("arg_req")->data[0];
	TEST_ASSERT_STREQ(arg_req_data0, "a",
		"Error while parsing flag \"arg_req\" - wrong argument 0"
		TEST_EXPECT_MSG(s), "0", arg_req_data0);
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
		TEST_EXPECT_MSG(s), "---bar", tlist_req_data0);
	char* tlist_req_data2 = cbuild_flag_get_flag("tlist_req")->data[2];
	TEST_ASSERT_STREQ(tlist_req_data2, "baz",
		"Error while parsing flag \"tlist_req\" - wrong argument 2"
		TEST_EXPECT_MSG(s), "baz", tlist_req_data0);
	
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
		"Error while parsing flag \"a1\" - flag should not be present"
		TEST_EXPECT_MSG(p), NULL, missing_flag1);
	cbuild_arglist_t* missing_flag2 = cbuild_flag_get_flag("a2");
	TEST_ASSERT_NEQ(missing_flag2, NULL,
		"Error while parsing flag \"a2\" - flag should not be present"
		TEST_EXPECT_MSG(p), NULL, missing_flag2);
	return 0;
}
