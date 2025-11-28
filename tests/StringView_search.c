int main(void) {
	// char ops
	cbuild_sv_t sv1 = cbuild_sv_from_lit("aaabacabaaa");
	// find
	ssize_t res11 = cbuild_sv_find(sv1, 'b');
	TEST_ASSERT_EQ(res11, 3, "cbuild_sv_find - wrong index for found character"
		TEST_EXPECT_MSG(zd), (ssize_t)3, res11);
	ssize_t res12 = cbuild_sv_find(sv1, 'z');
	TEST_ASSERT_EQ(res12, -1, "cbuild_sv_find - wrong error index"
		TEST_EXPECT_MSG(zd),	(ssize_t)-1, res12);
	ssize_t res13 = cbuild_sv_find(cbuild_sv_from_lit(""), 'z');
	TEST_ASSERT_EQ(res13, -1, "cbuild_sv_find - wrong error index on empty string"
		TEST_EXPECT_MSG(zd), (ssize_t)-1, res13);
	ssize_t res14 = cbuild_sv_find(sv1, 'a');
	TEST_ASSERT_EQ(res14, 0,
		"cbuild_sv_find - wrong index for first character search"
		TEST_EXPECT_MSG(zd), (ssize_t)0, res14);
	// rfind
	ssize_t res21 = cbuild_sv_rfind(sv1, 'b');
	TEST_ASSERT_EQ(res21, 7, "cbuild_sv_rfind - wrong index for found character"
		TEST_EXPECT_MSG(zd), (ssize_t)7, res21);
	ssize_t res22 = cbuild_sv_rfind(sv1, 'z');
	TEST_ASSERT_EQ(res22, -1, "cbuild_sv_rfind - wrong error index"
		TEST_EXPECT_MSG(zd),	(ssize_t)-1, res22);
	ssize_t res23 = cbuild_sv_rfind(cbuild_sv_from_lit(""), 'z');
	TEST_ASSERT_EQ(res23, -1, "cbuild_sv_rfind - wrong error index on empty string"
		TEST_EXPECT_MSG(zd), (ssize_t)-1, res23);
	ssize_t res24 = cbuild_sv_rfind(sv1, 'a');
	TEST_ASSERT_EQ(res24, 10,
		"cbuild_sv_rfind - wrong index for first character search"
		TEST_EXPECT_MSG(zd), (ssize_t)10, res24);
	// contains
	TEST_ASSERT(cbuild_sv_contains(sv1, 'c'),
		"cbuild_sv_contains: valid character not found.");
	TEST_NASSERT(cbuild_sv_contains(sv1, 'f'),
		"cbuild_sv_contains: found an unexpected character.");
	// sv ops
	cbuild_sv_t sv2 = cbuild_sv_from_lit("aaabacdcdcabaaa");
	// find_sv
	ssize_t res41 = cbuild_sv_find_sv(sv2, cbuild_sv_from_lit("dcd"));
	TEST_ASSERT_EQ(res41, 6, "cbuild_sv_find_sv - wrong index for found character"
		TEST_EXPECT_MSG(zd), (ssize_t)6, res41);
	ssize_t res42 = cbuild_sv_find_sv(sv2, cbuild_sv_from_lit("zcz"));
	TEST_ASSERT_EQ(res42, -1, "cbuild_sv_find_sv - wrong error index"
		TEST_EXPECT_MSG(zd),	(ssize_t)-1, res42);
	ssize_t res43 = cbuild_sv_find_sv(cbuild_sv_from_lit(""),
		cbuild_sv_from_lit("zcz"));
	TEST_ASSERT_EQ(res43, -1,
		"cbuild_sv_find_sv - wrong error index on empty string"
		TEST_EXPECT_MSG(zd), (ssize_t)-1, res43);
	ssize_t res44 = cbuild_sv_find_sv(sv2, cbuild_sv_from_lit("aa"));
	TEST_ASSERT_EQ(res44, 0,
		"cbuild_sv_find_sv - wrong index for first character search"
		TEST_EXPECT_MSG(zd), (ssize_t)0, res44);
	// contains_sv
	TEST_ASSERT(cbuild_sv_contains_sv(sv2, cbuild_sv_from_lit("dcd")),
		"cbuild_sv_contains_sv: valid substring not found.");
	TEST_NASSERT(cbuild_sv_contains_sv(sv2, cbuild_sv_from_lit("zcz")),
		"cbuild_sv_contains_sv: found an unexpected substring.");
	return 0;
}
