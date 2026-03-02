#pragma once // For LSP 
//! Implements dynamic array datatype. It works over structures with
//! specific "form", where `T` is a generic type:
//!
//! ```c
//! typedef struct cbuild_da_t
//!     T* data;
//!     size_t size;
//!     size_t capacity;
//! }
//! ```

/// Create type for a new dynamic array.
///
/// * [pl:type:type] Type of elements for a new dynamic array.
#define cbuild_da_new(type) \
	struct {                  \
		type* data;             \
		size_t size;            \
		size_t capacity;        \
	}
/// Append new element to dynamic array.
///
/// * [pl:da:cbuild_da_t*] Dynamic array object.
/// * [pl:elem:*typeof(da->data)] Element to append.
#define cbuild_da_append(da, elem)                                             \
	do {                                                                         \
		if (((da)->size + 1) > (da)->capacity) {                                   \
			cbuild_da_resize((da), 0);                                               \
		}                                                                          \
		(da)->data[(da)->size++] = elem;                                           \
	} while(0)
/// Append new elements to dynamic array.
///
/// * [pl:da:cbuild_da_t*] Dynamic array object.
/// * [pl:arr:typeof(da->data)] Elements to append.
/// * [pl:arr_size:size_t] Number of elements to append.
#define cbuild_da_append_arr(da, arr, arr_size)                                \
	do {                                                                         \
		if (((da)->size + (arr_size)) > (da)->capacity) {                          \
			cbuild_da_resize((da), (da)->capacity + (arr_size));                     \
		}                                                                          \
		memcpy((da)->data + (da)->size, (arr),                                     \
			(arr_size) * sizeof(typeof(*(da)->data)));                               \
		(da)->size += (arr_size);                                                  \
	} while(0)
/// Append new elements to dynamic array.
///
/// * [pl:da:cbuild_da_t*] Dynamic array object.
/// * [pl:...:*typeof(da->data)] Elements to append.
#define cbuild_da_append_many(da, ...)                                         \
	do {                                                                         \
		typeof(*((da)->data))   __cbuild_da_tmp_arr[] = { __VA_ARGS__ };           \
		size_t __cbuild_da_tmp_count =                                             \
			sizeof(__cbuild_da_tmp_arr) / sizeof(__cbuild_da_tmp_arr[0]);            \
		cbuild_da_append_arr((da), __cbuild_da_tmp_arr, __cbuild_da_tmp_count);    \
	} while (0)
/// Set element in dynamic array using its index.
///
/// * [pl:da:cbuild_da_t*] Dynamic array object.
/// * [pl:idx:size_t] Element index.
/// * [pl:elem:*typeof(da->data)] New element's value.
/// 
/// [r:bool] False on overflow.
#define cbuild_da_set(da, idx, elem)                                           \
	({                                                                           \
		bool __cbuild__ret = false;                                                \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR, "Index overflow in set.");                  \
			__cbuild__ret = false;                                                   \
		} else {                                                                   \
			(da)->data[(idx)] = elem;                                                \
			__cbuild__ret = true;                                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/// Get element from dynamic array using its index.
///
/// * [pl:da:const cbuild_da_t*] Dynamic array object.
/// * [pl:idx:size_t] Element index.
/// 
/// [r:typeof(da->data)] Pointer to an element or `NULL`{.c} if index out of bounds.
#define cbuild_da_get(da, idx)                                                 \
	({                                                                           \
		typeof(*(da)->data)* __cbuild__ret = NULL;                                 \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR, "Index overflow in get.");                  \
			__cbuild__ret = NULL;                                                    \
		} else {                                                                   \
			__cbuild__ret = &((da)->data[(idx)]);                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/// Remove element dynamic array using its index.
///
/// * [pl:da:cbuild_da_t*] Dynamic array object.
/// * [pl:idx:size_t] Element index.
/// 
/// [r:bool] False on overflow.
#define cbuild_da_remove(da, idx)                                              \
	({                                                                           \
		bool __cbuild__ret = false;                                                \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR, "Index overflow in remove.");               \
			__cbuild__ret = false;                                                   \
		} else {                                                                   \
			memmove((da)->data + (idx), ((da)->data + ((idx) + 1)),                  \
				((da)->size - ((idx) + 1)) * sizeof(typeof(*(da)->data)));             \
			(da)->size--;                                                            \
			__cbuild__ret = true;                                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/// Remove element dynamic array using its index.
///
/// This function has O(1) complexity but changes ordering of elements in 
/// an array. It copies last element into same index as removed one, overriding
/// it and then just decrease size by 1.
///
/// * [pl:da:cbuild_da_t*] Dynamic array object.
/// * [pl:idx:size_t] Element index.
/// 
/// [r:bool] False on overflow.
#define cbuild_da_remove_unordered(da, idx)                                    \
	({                                                                           \
		bool __cbuild__ret = false;                                                \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR, "Index overflow in remove.");               \
			__cbuild__ret = false;                                                   \
		} else {                                                                   \
			(da)->data[(idx)] = (da)->data[--(da)->size];                            \
			__cbuild__ret = true;                                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/// Resize dynamic array.
///
/// * [pl:da:cbuild_da_t*] Dynamic array object.
/// * [pl:new_size:size_t] Resize dynamic array. If new_size equal 0 default growth strategy will be used.
#define cbuild_da_resize(da, new_size)                                         \
	do {                                                                         \
		if ((da)->data == NULL) {                                                  \
			if ((new_size) == 0) {                                                   \
				(da)->capacity = CBUILD_INIT_CAPACITY;                                 \
			} else {                                                                 \
				(da)->capacity = (new_size);                                           \
			}                                                                        \
			(da)->data = __CBUILD_MALLOC(                                            \
				(da)->capacity * sizeof(typeof(*(da)->data)));                         \
		} else {                                                                   \
			if ((new_size) == 0) {                                                   \
				(da)->capacity *= 2;                                                   \
			} else {                                                                 \
				(da)->capacity = (new_size);                                           \
			}                                                                        \
			(da)->data = __CBUILD_REALLOC((da)->data,                                \
				(da)->capacity * sizeof(typeof(*(da)->data)));                         \
		}                                                                          \
		cbuild_assert((da)->data != NULL, "Allocation failed.\n");                 \
		((da)->size > (da)->capacity) ? ((da)->size = (da)->capacity) : 0;         \
	} while(0)
/// Free dynamic array.
///
/// This should be used only if you want to free underlying memory, if you just
/// want to clear array and reuse it, setting `size` to `0` would be much faster.
///
/// * [pl:da:cbuild_da_t*] Dynamic array object.
#define cbuild_da_clear(da)                                                    \
	do {                                                                         \
		__CBUILD_FREE((da)->data);                                                 \
		(da)->data     = NULL;                                                     \
		(da)->size     = 0;                                                        \
		(da)->capacity = 0;                                                        \
	} while(0)
/// Foreach implementation for dynamic array. Same semantics as 'for' loop.
///
/// * [pl:da:cbuild_da_t*] Dynamic array object.
/// * [pl:iter:name] Name if variable that will be used as iterator. Will have type `typeof(da->data)`{.c}.
#define cbuild_da_foreach(da, iter)                                            \
	for (typeof(*((da).data))* iter = (da).data;                                 \
		iter < ((da).data + (da).size); iter++)
