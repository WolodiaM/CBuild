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
// Code
cbuild_map_t(int, int, int, int);
bool   cbuild_map_int_int_keycmp(int* k1, int* k2) { return *k1 == *k2; }
size_t cbuild_map_int_int_hash(int* k) { return *k; }
cbuild_map_t_impl(int, int, int, int);
size_t talloc_last_alloc_size = 0;
void*  talloc_last_alloc_ptr  = NULL;
void*  talloc(size_t num) {
  talloc_last_alloc_size = num;
  void* ptr              = malloc(num);
  talloc_last_alloc_ptr  = ptr;
  return ptr;
}
void* tfree_last_free_ptr = NULL;
void  tfree(void* ptr) {
  tfree_last_free_ptr = ptr;
  free(ptr);
}
TEST_MAIN(
		{
			TEST_CASE(
					{
						cbuild_map_int_int_t map = cbuild_map_int_int;
						map.malloc               = talloc;
						map.free                 = tfree;
						cbuild_map_resize(&map, 256);
						TEST_ASSERT_EQ(
								talloc_last_alloc_size,
								256ul * sizeof(cbuild_map_int_int_bucket_t),
								"Wrong allocation value on resize" TEST_EXPECT_MSG(zu),
								256ul * sizeof(cbuild_map_int_int_bucket_t),
								talloc_last_alloc_size);
						TEST_ASSERT_EQ(
								map.nbuckets, 256,
								"Wrong bucket count was set on resize" TEST_EXPECT_MSG(zu),
								(size_t)256, map.nbuckets);
						cbuild_map_clear(&map);
						TEST_ASSERT_EQ(tfree_last_free_ptr, talloc_last_alloc_ptr,
		                       "Wrong pointer was sent to free" TEST_EXPECT_MSG(p),
		                       talloc_last_alloc_ptr, tfree_last_free_ptr);
						TEST_ASSERT_EQ(
								map.nbuckets, 0,
								"Wrong bucket count was set on clear" TEST_EXPECT_MSG(zu),
								(size_t)0, map.nbuckets);
					},
					"Map allocation sequence");
			TEST_CASE(
					{
						cbuild_map_int_int_t map = cbuild_map_int_int;
						printf("\tWrites without hash collisions\n");
						cbuild_map_resize(&map, 256);
						cbuild_map_set(&map, 0, 1);
						cbuild_map_set(&map, 1, 2);
						TEST_ASSERT_NEQ(
								map.buckets[0].vals, NULL, "%s",
								"Key 0 must be hashed in bucket 0 but bucket is empty.");
						TEST_ASSERT_EQ(
								map.buckets[0].nvals, 1,
								"Wrong element count was set in bucket 0" TEST_EXPECT_MSG(zu),
								(size_t)1, map.buckets[0].nvals);
						TEST_ASSERT_EQ(
								map.buckets[0].vals[0].val, 1,
								"Wrong element was written into bucket 0" TEST_EXPECT_MSG(d), 1,
								map.buckets[0].vals[0].val);
						TEST_ASSERT_NEQ(
								map.buckets[1].vals, NULL, "%s",
								"Key 1 must be hashed in bucket 1 but bucket is empty.");
						TEST_ASSERT_EQ(
								map.buckets[1].nvals, 1,
								"Wrong element count was set in bucket 1" TEST_EXPECT_MSG(zu),
								(size_t)1, map.buckets[1].nvals);
						TEST_ASSERT_EQ(
								map.buckets[1].vals[0].val, 2,
								"Wrong element was written into bucket 1" TEST_EXPECT_MSG(d), 2,
								map.buckets[1].vals[0].val);
						cbuild_map_clear(&map);
						printf("\tWrites with hash collisions\n");
						cbuild_map_resize(&map, 256);
						cbuild_map_set(&map, 0, 1);
						cbuild_map_set(&map, 256, 2);
						TEST_ASSERT_NEQ(map.buckets[0].vals, NULL, "%s",
		                        "Key 0 and 256 must be hashed in bucket 0 but "
		                        "bucket is empty.");
						TEST_ASSERT_EQ(
								map.buckets[0].nvals, 2,
								"Wrong element count was set in bucket 0" TEST_EXPECT_MSG(zu),
								(size_t)2, map.buckets[0].nvals);
						TEST_ASSERT_EQ(map.buckets[0].vals[0].val, 1,
		                       "Wrong element was written into bucket 0 value "
		                       "0" TEST_EXPECT_MSG(d),
		                       1, map.buckets[0].vals[0].val);
						TEST_ASSERT_EQ(map.buckets[0].vals[1].val, 2,
		                       "Wrong element was written into bucket 0 value "
		                       "1" TEST_EXPECT_MSG(d),
		                       2, map.buckets[0].vals[1].val);
						cbuild_map_clear(&map);
						printf("\tWrites with overwrite\n");
						cbuild_map_resize(&map, 256);
						cbuild_map_set(&map, 0, 1);
						cbuild_map_set(&map, 0, 2);
						TEST_ASSERT_NEQ(map.buckets[0].vals, NULL, "%s",
		                        "Key 0 and 256 must be hashed in bucket 0 but "
		                        "bucket is empty.");
						TEST_ASSERT_EQ(
								map.buckets[0].nvals, 1,
								"Wrong element count was set in bucket 0" TEST_EXPECT_MSG(zu),
								(size_t)1, map.buckets[0].nvals);
						TEST_ASSERT_EQ(map.buckets[0].vals[0].val, 2,
		                       "Wrong element was written into bucket 0 value "
		                       "0" TEST_EXPECT_MSG(d),
		                       2, map.buckets[0].vals[0].val);
						cbuild_map_clear(&map);
					},
					"Map writing");
			TEST_CASE(
					{
						cbuild_map_int_int_t map = cbuild_map_int_int;
						printf("\tReads without hash collisions\n");
						cbuild_map_resize(&map, 256);
						cbuild_map_set(&map, 0, 1);
						cbuild_map_set(&map, 1, 2);
						TEST_ASSERT_EQ(*cbuild_map_get(&map, 0), 1,
		                       "Wrong element read" TEST_EXPECT_MSG(d), 1,
		                       *cbuild_map_get(&map, 0));
						TEST_ASSERT_EQ(*cbuild_map_get(&map, 1), 2,
		                       "Wrong element read" TEST_EXPECT_MSG(d), 2,
		                       *cbuild_map_get(&map, 1));
						TEST_ASSERT(cbuild_map_contains(&map, 0), "%s",
		                    "Map contains is broken.");
						TEST_NASSERT(cbuild_map_contains(&map, 3), "%s",
		                     "Map contains is broken.");
						cbuild_map_clear(&map);
						printf("\tReads with hash collisions\n");
						cbuild_map_resize(&map, 1);
						cbuild_map_set(&map, 0, 1);
						cbuild_map_set(&map, 1, 2);
						TEST_ASSERT_EQ(*cbuild_map_get(&map, 0), 1,
		                       "Wrong element read" TEST_EXPECT_MSG(d), 1,
		                       *cbuild_map_get(&map, 0));
						TEST_ASSERT_EQ(*cbuild_map_get(&map, 1), 2,
		                       "Wrong element read" TEST_EXPECT_MSG(d), 2,
		                       *cbuild_map_get(&map, 1));
						TEST_ASSERT(cbuild_map_contains(&map, 0), "%s",
		                    "Map contains is broken.");
						TEST_NASSERT(cbuild_map_contains(&map, 3), "%s",
		                     "Map contains is broken.");
						cbuild_map_clear(&map);
					},
					"Map reading");
			TEST_CASE(
					{
						cbuild_map_int_int_t map = cbuild_map_int_int;
						cbuild_map_resize(&map, 256);
						cbuild_map_set(&map, 0, 1);
						cbuild_map_set(&map, 1, 2);
						cbuild_map_remove(&map, 1);
						cbuild_map_remove(&map, 2);
						TEST_ASSERT_EQ(*cbuild_map_get(&map, 0), 1,
		                       "Wrong element read" TEST_EXPECT_MSG(d), 1,
		                       *cbuild_map_get(&map, 0));
						TEST_ASSERT_EQ(cbuild_map_get(&map, 1), NULL,
		                       "Wrong element read" TEST_EXPECT_MSG(p), NULL,
		                       (void*)cbuild_map_get(&map, 1));
						cbuild_map_clear(&map);
					},
					"Removing from a map");
			TEST_CASE(
					{
						cbuild_map_int_int_t map = cbuild_map_int_int;
						cbuild_map_resize(&map, 256);
						cbuild_map_set(&map, 0, 1);
						cbuild_map_set(&map, 3, 4);
						cbuild_map_int_int_iter_t  iter   = cbuild_map_get_iterator(&map);
						bool                       found0 = false;
						bool                       found1 = false;
						cbuild_map_int_int_pair_t* pair   = cbuild_map_iter_next(&iter);
						while (pair != NULL) {
							if (pair->key == 0 && pair->val == 1) {
								found0 = true;
							} else if (pair->key == 3 && pair->val == 4) {
								found1 = true;
							}
							pair = cbuild_map_iter_next(&iter);
						}
						TEST_ASSERT(found0, "%s", "Element with key 0 was not iterated.");
						TEST_ASSERT(found1, "%s", "Element with key 1 was not iterated.");
						cbuild_map_clear(&map);
					},
					"Iterating over a map");
			TEST_CASE(
					{
						cbuild_map_int_int_t map = cbuild_map_int_int;
						cbuild_map_resize(&map, 256);
						cbuild_map_set(&map, 0, 1);
						cbuild_map_set(&map, 1, 2);
						size_t num   = 0;
						int*   klist = cbuild_map_keylist(&map, &num);
						TEST_ASSERT_EQ(num, 2, "Wrong number of keys" TEST_EXPECT_MSG(zu),
		                       (size_t)2, num);
						TEST_ASSERT_NEQ(klist, NULL, "%s", "Array was not allocated.");
						TEST_ASSERT_EQ(klist[0], 0,
		                       "Wrong key at index 0" TEST_EXPECT_MSG(d), 0,
		                       klist[0]);
						TEST_ASSERT_EQ(klist[1], 1,
		                       "Wrong key at index 1" TEST_EXPECT_MSG(d), 1,
		                       klist[1]);
						free(klist);
						cbuild_map_clear(&map);
						klist = cbuild_map_keylist(&map, &num);
						TEST_ASSERT_EQ(num, 0, "Wrong number of keys" TEST_EXPECT_MSG(zu),
		                       (size_t)0, num);
						TEST_ASSERT_NEQ(klist, NULL, "%s", "Array was not malloced.");
						free(klist);
					},
					"Listing map keys");
		},
		"Map data structure tests")
