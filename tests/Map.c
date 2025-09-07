/**
 * @file Map.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for map datatype
 *
 * @date 2025-06-20
 * @copyright (C) 2025 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2025 WolodiaM
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// Project includes
#include "../src/Map.h"
#include "framework.h"
#include <string.h>
// Code
size_t test_int_hash(const void* map, const void* key) {
	CBUILD_UNUSED(map);
	int k = *((int*)key);
	return (size_t)k;
}
size_t test_cstr_hash(const void* map, const void* key) {
	CBUILD_UNUSED(map);
	char* k = *((char**)key);
	return (size_t)(*k);
}
void test_cstr_free(const void* map, void* elem) {
	CBUILD_UNUSED(map);
	free(*((char**)elem));
}
char* test_strdup(char* str) {
	size_t len = strlen(str);
	char* ret = malloc(len + 1);
	memcpy(ret, str, len);
	ret[len] = '\0';
	return ret;
}
// This tests uses two variants of a map - first is 'int->int' second is
// 'cstr->intptr_t'.
// In first case I use int* as an element. So key is elem[0] and value is
// elem[1];. To test I cast bucket values to an 'int*'. So key will be
// vals[element_id*2+0] and element is vals[element_id*2+1].
// In second case a void* is received from a map. Then it is casted either
// to char** and then [0] is read or to intptr_t* and then [1] is read.
// intptr_t has same size as the char* so it can be read very easily.
// The access patter is the same as in previous map, but now different type
// casts need to be used for different elements.
TEST_MAIN({
	_Static_assert(sizeof(char*) == sizeof(intptr_t),
	  "Cannot test cstr->int map on this platform");
	TEST_CASE(
	{
		cbuild_map_t map = {0};
		map.elem_size = 2 * sizeof(int);
		map.key_size = sizeof(int);
		cbuild_map_init(&map, 256);
		TEST_ASSERT_EQ(
		  map.nbuckets, 256,
		  "Wrong bucket count was set on resize" TEST_EXPECT_MSG(zu),
		  (size_t)256, map.nbuckets);
		TEST_ASSERT_NEQ(map.buckets, NULL, "%s",
		  "Buckets array was not allocated on init");
		cbuild_map_clear(&map);
		TEST_ASSERT_EQ(
		  map.nbuckets, 0,
		  "Wrong bucket count was set on clear" TEST_EXPECT_MSG(zu),
		  (size_t)0, map.nbuckets);
		TEST_ASSERT_EQ(map.buckets, NULL, "%s",
		  "Buckkets array was not free on clear");
	},
	"Map allocation sequence");
	TEST_CASE(
	{
		cbuild_map_t map = {0};
		map.elem_size = 2 * sizeof(int);
		map.key_size = sizeof(int);
		map.hash_func = test_int_hash;
		int* elem;
		printf("\tWrites without hash collisions\n");
		cbuild_map_init(&map, 256);
		elem = cbuild_map_get_or_alloc(&map, 0);
		elem[0] = 0; elem[1] = 1;
		elem = cbuild_map_get_or_alloc(&map, 1);
		elem[0] = 1; elem[1] = 2;
		TEST_ASSERT_NEQ(
		  map.buckets[0].vals, NULL, "%s",
		  "Key 0 must be hashed in bucket 0 but bucket is empty.");
		TEST_ASSERT_EQ(
		  map.buckets[0].nvals, 1,
		  "Wrong element count was set in bucket 0" TEST_EXPECT_MSG(zu),
		  (size_t)1, map.buckets[0].nvals);
		TEST_ASSERT_EQ(
		  ((int*)map.buckets[0].vals)[1], 1,
		  "Wrong element was written into bucket 0" TEST_EXPECT_MSG(d), 1,
		  ((int*)map.buckets[0].vals)[1]);
		TEST_ASSERT_NEQ(
		  map.buckets[1].vals, NULL, "%s",
		  "Key 1 must be hashed in bucket 1 but bucket is empty.");
		TEST_ASSERT_EQ(
		  map.buckets[1].nvals, 1,
		  "Wrong element count was set in bucket 1" TEST_EXPECT_MSG(zu),
		  (size_t)1, map.buckets[1].nvals);
		TEST_ASSERT_EQ(
		  ((int*)map.buckets[1].vals)[1], 2,
		  "Wrong element was written into bucket 1" TEST_EXPECT_MSG(d), 2,
		  ((int*)map.buckets[1].vals)[1]);
		cbuild_map_clear(&map);
		printf("\tWrites with hash collisions\n");
		cbuild_map_init(&map, 256);
		elem = cbuild_map_get_or_alloc(&map, 0);
		elem[0] = 0; elem[1] = 1;
		elem = cbuild_map_get_or_alloc(&map, 256);
		elem[0] = 256; elem[1] = 2;
		TEST_ASSERT_NEQ(map.buckets[0].vals, NULL, "%s",
		  "Key 0 and 256 must be hashed in bucket 0 but "
		  "bucket is empty.");
		TEST_ASSERT_EQ(
		  map.buckets[0].nvals, 2,
		  "Wrong element count was set in bucket 0" TEST_EXPECT_MSG(zu),
		  (size_t)2, map.buckets[0].nvals);
		TEST_ASSERT_EQ(((int*)map.buckets[0].vals)[1], 1,
		  "Wrong element was written into bucket 0 value "
		  "0" TEST_EXPECT_MSG(d),
		  1, ((int*)map.buckets[0].vals)[1]);
		TEST_ASSERT_EQ(((int*)map.buckets[0].vals)[3], 2,
		  "Wrong element was written into bucket 0 value "
		  "1" TEST_EXPECT_MSG(d),
		  2, ((int*)map.buckets[0].vals)[3]);
		cbuild_map_clear(&map);
		printf("\tWrites with overwrite\n");
		cbuild_map_init(&map, 256);
		elem = cbuild_map_get_or_alloc(&map, 0);
		elem[0] = 0; elem[1] = 1;
		elem = cbuild_map_get_or_alloc(&map, 0);
		elem[0] = 0; elem[1] = 2;
		TEST_ASSERT_NEQ(map.buckets[0].vals, NULL, "%s",
		  "Value was not written at all.");
		TEST_ASSERT_EQ(
		  map.buckets[0].nvals, 1,
		  "Wrong element count was set in bucket 0" TEST_EXPECT_MSG(zu),
		  (size_t)1, map.buckets[0].nvals);
		TEST_ASSERT_EQ(((int*)map.buckets[0].vals)[1], 2,
		  "Wrong element was written into bucket 0 value "
		  "0" TEST_EXPECT_MSG(d),
		  2, ((int*)map.buckets[0].vals)[1]);
		cbuild_map_clear(&map);
	},
	"Writing to an int->int map");
	TEST_CASE(
	{
		cbuild_map_t map = {0};
		map.elem_size = sizeof(char*) + sizeof(intptr_t);
		map.key_size = 0;
		map.hash_func = test_cstr_hash;
		char* key;
		void* elem;
		printf("\tWrites without hash collisions\n");
		cbuild_map_init(&map, 256);
		key = "abc";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
		key = "def";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
		TEST_ASSERT_NEQ(
		  map.buckets['a'].vals, NULL,
		  "Key \"abc\" must be hashed in bucket %d but bucket is empty.", 'a');
		TEST_ASSERT_EQ(
		  map.buckets['a'].nvals, 1,
		  "Wrong element count was set in bucket %d" TEST_EXPECT_MSG(zu),
		  'a', (size_t)1, map.buckets['a'].nvals);
		TEST_ASSERT_EQ(
		  ((intptr_t*)map.buckets['a'].vals)[1], 1,
		  "Wrong element was written into bucket %d" TEST_EXPECT_RMSG("%"PRIu64),
		  'a', (uint64_t)1, ((intptr_t*)map.buckets['a'].vals)[1]);
		TEST_ASSERT_NEQ(
		  map.buckets['d'].vals, NULL,
		  "Key \"def\" must be hashed in bucket %d but bucket is empty.", 'd');
		TEST_ASSERT_EQ(
		  map.buckets['d'].nvals, 1,
		  "Wrong element count was set in bucket %d" TEST_EXPECT_MSG(zu),
		  'd', (size_t)1, map.buckets['d'].nvals);
		TEST_ASSERT_EQ(
		  ((intptr_t*)map.buckets['d'].vals)[1], 2,
		  "Wrong element was written into bucket %d" TEST_EXPECT_RMSG("%"PRIu64),
		  'd', (uint64_t)2, ((intptr_t*)map.buckets['d'].vals)[1]);
		free(((char**)map.buckets['a'].vals)[0]);
		free(((char**)map.buckets['d'].vals)[0]);
		cbuild_map_clear(&map);
		printf("\tWrites with hash collisions\n");
		cbuild_map_init(&map, 256);
		key = "abc";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
		key = "aef";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
		TEST_ASSERT_NEQ(map.buckets['a'].vals, NULL,
		  "Key 0 and 256 must be hashed in bucket %d but bucket is empty.", 'a');
		TEST_ASSERT_EQ(
		  map.buckets['a'].nvals, 2,
		  "Wrong element count was set in bucket %d" TEST_EXPECT_MSG(zu),
		  'a', (size_t)2, map.buckets['a'].nvals);
		TEST_ASSERT_EQ(((intptr_t*)map.buckets['a'].vals)[1], 1,
		  "Wrong element was written into bucket %d value 0" TEST_EXPECT_RMSG("%"PRIu64),
		  'a', (uint64_t)1, ((intptr_t*)map.buckets['a'].vals)[1]);
		TEST_ASSERT_EQ(((intptr_t*)map.buckets['a'].vals)[3], 2,
		  "Wrong element was written into bucket %d value 1" TEST_EXPECT_RMSG("%"PRIu64),
		  'a', (uint64_t)2, ((intptr_t*)map.buckets['a'].vals)[3]);
		free(((char**)map.buckets['a'].vals)[0]);
		free(((char**)map.buckets['a'].vals)[2]);
		cbuild_map_clear(&map);
		printf("\tWrites with overwrite\n");
		cbuild_map_init(&map, 256);
		key = "abc";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
		key = "abc";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		free(((char**)elem)[0]);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
		TEST_ASSERT_NEQ(map.buckets['a'].vals, NULL, "%s",
		  "Value was not written at all.");
		TEST_ASSERT_EQ(
		  map.buckets['a'].nvals, 1,
		  "Wrong element count was set in bucket %d" TEST_EXPECT_MSG(zu),
		  'a', (size_t)1, map.buckets[0].nvals);
		TEST_ASSERT_EQ(((intptr_t*)map.buckets['a'].vals)[1], 2,
		  "Wrong element was written into bucket %d value 0" TEST_EXPECT_RMSG("%"PRIu64),
		  'a', (uint64_t)2, ((intptr_t*)map.buckets[0].vals)[1]);
		free(((char**)map.buckets['a'].vals)[0]);
		cbuild_map_clear(&map);
	},
	"Writing to an cstr->int map");
	TEST_CASE(
	{
		cbuild_map_t map = {0};
		map.elem_size = 2 * sizeof(int);
		map.key_size = sizeof(int);
		int* elem;
		printf("\tReads without hash collisions\n");
		cbuild_map_init(&map, 256);
		elem = cbuild_map_get_or_alloc(&map, 0);
		elem[0] = 0; elem[1] = 1;
		elem = cbuild_map_get_or_alloc(&map, 1);
		elem[0] = 1; elem[1] = 2;
		TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 0))[1], 1,
		  "Wrong element read" TEST_EXPECT_MSG(d), 1,
		  ((int*)cbuild_map_get(&map, 0))[1]);
		TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 1))[1], 2,
		  "Wrong element read" TEST_EXPECT_MSG(d), 2,
		  ((int*)cbuild_map_get(&map, 1))[1]);
		cbuild_map_clear(&map);
		printf("\tReads with hash collisions\n");
		map.hash_func = test_int_hash;
		cbuild_map_init(&map, 1);
		elem = cbuild_map_get_or_alloc(&map, 0);
		elem[0] = 0; elem[1] = 1;
		elem = cbuild_map_get_or_alloc(&map, 1);
		elem[0] = 1; elem[1] = 2;
		TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 0))[1], 1,
		  "Wrong element read" TEST_EXPECT_MSG(d), 1,
		  ((int*)cbuild_map_get(&map, 0))[1]);
		TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 1))[1], 2,
		  "Wrong element read" TEST_EXPECT_MSG(d), 2,
		  ((int*)cbuild_map_get(&map, 1))[1]);
		cbuild_map_clear(&map);
	},
	"Reading from an int->int map");
	TEST_CASE(
	{
		cbuild_map_t map = {0};
		map.elem_size = sizeof(char*) + sizeof(intptr_t);
		map.key_size = 0;
		char* key;
		void* elem;
		printf("\tReads without hash collisions\n");
		cbuild_map_init(&map, 256);
		key = "abc";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
		key = "def";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
		key = "abc";
		TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 1,
		  "Wrong element read" TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)1,
		  ((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
		key = "def";
		TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 2,
		  "Wrong element read" TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)2,
		  ((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
		cbuild_map_clear_ex(&map, test_cstr_free);
		printf("\tReads with hash collisions\n");
		map.hash_func = test_cstr_hash;
		cbuild_map_init(&map, 1);
		key = "abc";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
		key = "aef";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
		key = "abc";
		TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 1,
		  "Wrong element read" TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)1,
		  ((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
		key = "aef";
		TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 2,
		  "Wrong element read" TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)2,
		  ((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
		cbuild_map_clear_ex(&map, test_cstr_free);
	},
	"Reading from an cstr->int map");
	TEST_CASE(
	{
		cbuild_map_t map = {0};
		map.elem_size = 2 * sizeof(int);
		map.key_size = sizeof(int);
		int* elem;
		cbuild_map_init(&map, 256);
		elem = cbuild_map_get_or_alloc(&map, 0);
		elem[0] = 0; elem[1] = 1;
		elem = cbuild_map_get_or_alloc(&map, 1);
		elem[0] = 1; elem[1] = 2;
		cbuild_map_remove(&map, 1);
		cbuild_map_remove(&map, 2);
		TEST_ASSERT_EQ(((int*)cbuild_map_get(&map, 0))[1], 1,
		  "Wrong element read" TEST_EXPECT_MSG(d), 1,
		  ((int*)cbuild_map_get(&map, 0))[1]);
		TEST_ASSERT_EQ(cbuild_map_get(&map, 1), NULL,
		  "Wrong element read" TEST_EXPECT_MSG(p), NULL,
		  (void*)cbuild_map_get(&map, 1));
		cbuild_map_clear(&map);
	},
	"Removing from an int->int map");
	TEST_CASE(
	{
		cbuild_map_t map = {0};
		map.elem_size = sizeof(char*) + sizeof(intptr_t);
		map.key_size = 0;
		char* key;
		void* elem;
		cbuild_map_init(&map, 256);
		key = "abc";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup("abc"); ((intptr_t*)elem)[1] = 1;
		key = "def";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 2;
		key = "aaa";
		cbuild_map_remove_ex_ptr(&map, &key, test_cstr_free);
		key = "def";
		cbuild_map_remove_ex_ptr(&map, &key, test_cstr_free);
		key = "abc";
		TEST_ASSERT_EQ(((intptr_t*)cbuild_map_get_ptr(&map, &key))[1], 1,
		  "Wrong element read" TEST_EXPECT_RMSG("%"PRIu64), (uint64_t)1,
		  ((intptr_t*)cbuild_map_get_ptr(&map, &key))[1]);
		key =	"def";
		TEST_ASSERT_EQ(cbuild_map_get_ptr(&map, &key), NULL,
		  "Wrong element read" TEST_EXPECT_MSG(p), NULL,
		  (void*)cbuild_map_get_ptr(&map, &key));
		cbuild_map_clear_ex(&map, test_cstr_free);
	},
	"Removing from an cstr->int map");
	TEST_CASE(
	{
		cbuild_map_t map = {0};
		map.elem_size = 2 * sizeof(int);
		map.key_size = sizeof(int);
		int* elem;
		cbuild_map_init(&map, 256);
		elem = cbuild_map_get_or_alloc(&map, 0);
		elem[0] = 0; elem[1] = 1;
		elem = cbuild_map_get_or_alloc(&map, 3);
		elem[0] = 3; elem[1] = 4;
		bool found0 = false;
		bool found3 = false;
		cbuild_map_foreach(&map, int, iter) {
			if(iter[0] == 0) found0 = true;
			if(iter[0] == 3) found3 = true;
		}
		TEST_ASSERT(found0, "%s", "Element with key 0 was not iterated.");
		TEST_ASSERT(found3, "%s", "Element with key 3 was not iterated.");
		cbuild_map_clear(&map);
	},
	"Iterating over an int->int map");
	TEST_CASE(
	{
		cbuild_map_t map = {0};
		map.elem_size = sizeof(char*) + sizeof(intptr_t);
		map.key_size = 0;
		char* key;
		void* elem;
		cbuild_map_init(&map, 256);
		key = "abc";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 1;
		key = "def";
		elem = cbuild_map_get_or_alloc_ptr(&map, &key);
		((char**)elem)[0] = test_strdup(key); ((intptr_t*)elem)[1] = 4;
		bool found_abc = false;
		bool found_def = false;
		cbuild_map_foreach_raw(&map, iter) {
			if(strcmp(((char**)iter)[0], "abc") == 0) found_abc = true;
			if(strcmp(((char**)iter)[0], "def") == 0) found_def = true;
		}
		TEST_ASSERT(found_abc, "%s", "Element with key \"abc\" was not iterated.");
		TEST_ASSERT(found_def, "%s", "Element with key \"def\" was not iterated.");
		cbuild_map_clear_ex(&map, test_cstr_free);
	},
	"Iterating over an cstr->int map");
},
"Map data structure tests")
