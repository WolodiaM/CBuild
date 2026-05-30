//! Linked list. For merge with CBuild
//!
//! License: GPL-3.0-or-later

#include "LL.h"
#include "Common.h"
#include "Allocator.h"

typedef struct __cbuild_ll_node_t {
	void* prev;
	void* next;
} __cbuild_ll_node_t;
CBUILDDEF __cbuild_ll_node_t* __cbuild_ll_get_state(void* data) {
	// In C, pointer arithmetic account for array type.
	return ((__cbuild_ll_node_t*)data) - 1;
}
CBUILDDEF void* cbuild_ll_new(cbuild_allocator_t* a, void* elem, size_t size) {
	__cbuild_ll_node_t* node = a->malloc(a, sizeof(__cbuild_ll_node_t) + size);
	cbuild_assert(node != NULL, "Allocation failed.\n");
	if (elem == NULL) {
		node->prev = NULL;
		node->next = NULL;
	} else {
		void* tail = cbuild_ll_tail(elem);
		__cbuild_ll_get_state(tail)->next = node + 1; //  Make it point to data.
		node->prev = tail;
		node->next = NULL;
	}
	memset(node + 1, 0, size);
	return node + 1; // Return pointer to data.
}
CBUILDDEF void* cbuild_ll_append_raw(cbuild_allocator_t* a, void* elem, void* data, size_t size) {
	void* node = cbuild_ll_new(a, elem == NULL ? NULL : *(void**)elem, size);
	memcpy(node, data, size);
	if (elem != NULL && *(void**)elem == NULL) *(void**)elem = node;
	return node;
}
CBUILDDEF size_t cbuild_ll_size(void* elem) {
	if (elem == NULL) return 0;
	void* head = cbuild_ll_head(elem);
	size_t count = 1;
	while (__cbuild_ll_get_state(head)->next != NULL) {
		head = __cbuild_ll_get_state(head)->next;
		count++;
	}
	return count;
}
CBUILDDEF void cbuild_ll_remove(cbuild_allocator_t* a, void* elem) {
	if (elem == NULL) return;
	void* prev = __cbuild_ll_get_state(elem)->prev;
	void* next = __cbuild_ll_get_state(elem)->next;
	if (prev != NULL) __cbuild_ll_get_state(prev)->next = next;
	if (next != NULL) __cbuild_ll_get_state(next)->prev = prev;
	a->free(a, __cbuild_ll_get_state(elem));
}
CBUILDDEF void cbuild_ll_free(cbuild_allocator_t* a, void* elem) {
	void* tail = cbuild_ll_tail(elem);
	while (tail != NULL) {
		void* new_tail = __cbuild_ll_get_state(tail)->prev;
		a->free(a, __cbuild_ll_get_state(tail));
		tail = new_tail;
	}
}
CBUILDDEF void* cbuild_ll_next(void* elem) {
	if (elem == NULL) return NULL;
	return __cbuild_ll_get_state(elem)->next;
}
CBUILDDEF void* cbuild_ll_prev(void* elem) {
	if (elem == NULL) return NULL;
	return __cbuild_ll_get_state(elem)->prev;
}
CBUILDDEF void* cbuild_ll_head(void* elem) {
	if (elem == NULL) return NULL;
	void* head = elem;
	while (__cbuild_ll_get_state(head)->prev != NULL) {
		head = __cbuild_ll_get_state(head)->prev;
	}
	return head;
}
CBUILDDEF void* cbuild_ll_tail(void* elem) {
	if (elem == NULL) return NULL;
	void* tail = elem;
	while (__cbuild_ll_get_state(tail)->next!= NULL) {
		tail = __cbuild_ll_get_state(tail)->next;
	}
	return tail;
}
