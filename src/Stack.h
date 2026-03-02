#pragma once // For LSP 
//! Stack datatype implementation. This implements same overall datastructure
//! as dynamic array but with simplified API to only allow `push`, `pop` and
//! `top` (just read top element without poping it) operations. It works over
//! structures with specific "form", where `T` is a generic type:
//!
//! ```c
//! typedef struct cbuild_da_t
//!     T* data;
//!     size_t ptr;
//!     size_t capacity;
//! }
//! ```

/// Push a new element on top of a stack.
///
/// * [pl:stack:cbuild_stack_t*] Stack object.
/// * [pl:elem:*typeof(stack->data)] New element.
#define cbuild_stack_push(stack, elem)                                         \
	do {                                                                         \
		if ((stack)->ptr >= (stack)->capacity) {                                   \
			cbuild_stack_resize((stack), 0);                                         \
		}                                                                          \
		(stack)->data[(stack)->ptr++] = elem;                                      \
	} while(0)
/// Pop element from top of a stack.
///
/// * [pl:stack:cbuild_stack_t*] Stack object.
/// 
/// [r:*typeof(stack->data)] Popped element.
#define cbuild_stack_pop(stack)                                                \
	({                                                                           \
		cbuild_assert((stack)->ptr != 0, "Stack underfloor.\n");                   \
		(stack)->data[--(stack)->ptr];                                             \
	})
/// Get element from top of a stack.
///
/// * [pl:stack:cbuild_stack_t*] Stack object.
/// 
/// [r:*typeof(stack->data)] Element on top of a stack.
#define cbuild_stack_top(stack)                                                \
	({                                                                           \
		cbuild_assert((stack)->ptr != 0, "Stack underfloor.\n");                   \
		(stack)->data[(stack)->ptr - 1];                                           \
	})
/// Resize dynamic array.
///
/// * [pl:stack:cbuild_stack_t*] Stack object.
/// * [pl:new_size:size_t] Resize stack If new_size equal 0 default growth strategy will be used.
#define cbuild_stack_resize(stack, size)                                       \
	do {                                                                         \
		if ((stack)->data == NULL) {                                               \
			if (size == 0) {                                                         \
				(stack)->capacity = CBUILD_INIT_CAPACITY;                              \
			} else {                                                                 \
				(stack)->capacity = size;                                              \
			}                                                                        \
			(stack)->data = __CBUILD_MALLOC(                                         \
				(stack)->capacity * sizeof(typeof(*(stack)->data)));                   \
		} else {                                                                   \
			if (size == 0) {                                                         \
				(stack)->capacity *= 2;                                                \
			} else {                                                                 \
				(stack)->capacity = size;                                              \
			}                                                                        \
			(stack)->data = __CBUILD_REALLOC((stack)->data,                          \
				(stack)->capacity * sizeof(typeof(*(stack)->data)));                   \
		}                                                                          \
		cbuild_assert((stack)->data != NULL, "Allocation failed.\n");              \
	} while (0)
/// Free stack.
///
/// This should be used only if you want to free underlying memory, if you just
/// want to clear stack and reuse it, setting `ptr` to `0` would be much faster.
///
/// * [pl:stack:cbuild_stack_t*] Stack object.
#define cbuild_stack_clear(stack)                                              \
	do {                                                                         \
		__CBUILD_FREE((stack)->data);                                              \
		(stack)->data     = NULL;                                                  \
		(stack)->ptr      = 0;                                                     \
		(stack)->capacity = 0;                                                     \
	} while(0)
