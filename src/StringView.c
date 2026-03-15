#include "StringView.h"
#include "Common.h"
#include "Temp.h"
size_t cbuild_sv_trim_left(cbuild_sv_t* sv) {
	size_t i = 0;
	while(i < sv->size && isspace((unsigned char)sv->data[i])) {
		i++;
	}
	sv->data += i;
	sv->size -= i;
	return i;
}
size_t cbuild_sv_trim_right(cbuild_sv_t* sv) {
	size_t i = sv->size;
	while(i > 0 && isspace((unsigned char)sv->data[i - 1])) {
		i--;
	}
	size_t tmp = sv->size - i;
	sv->size  -= tmp;
	return tmp;
}
size_t cbuild_sv_trim(cbuild_sv_t* sv) {
	size_t ret = cbuild_sv_trim_left(sv);
	ret += cbuild_sv_trim_right(sv);
	return ret;
}
cbuild_sv_t cbuild_sv_chop(cbuild_sv_t* sv, size_t size) {
	if(size > sv->size) {
		size = sv->size;
	}
	char* tmp = sv->data;
	sv->data += size;
	sv->size -= size;
	return cbuild_sv_from_parts(tmp, size);
}
cbuild_sv_t cbuild_sv_chop_right(cbuild_sv_t* sv, size_t size) {
	if(size > sv->size) {
		size = sv->size;
	}
	sv->size -= size;
	return cbuild_sv_from_parts(sv->data + sv->size, size);
}
cbuild_sv_t cbuild_sv_chop_by_delim(cbuild_sv_t* sv, char delim) {
	char* chrptr = memchr(sv->data, delim, sv->size);
	if(chrptr != NULL) {
		size_t i = (size_t)(chrptr - sv->data);
		cbuild_sv_t ret = cbuild_sv_from_parts(sv->data, i);
		sv->data += i + 1;
		sv->size -= i + 1;
		return ret;
	}
	return cbuild_sv_chop(sv, sv->size);
}
cbuild_sv_t cbuild_sv_chop_right_by_delim(cbuild_sv_t* sv, char delim) {
	char* chrptr = __cbuild_memrchr(sv->data, delim, sv->size);
	if(chrptr != NULL) {
		size_t i = (size_t)(chrptr - sv->data);
		cbuild_sv_t ret = cbuild_sv_from_parts(chrptr + 1, (sv->size - i) - 1);
		sv->size -= (sv->size - i);
		return ret;
	}
	return cbuild_sv_chop(sv, sv->size);
}
cbuild_sv_t cbuild_sv_chop_by_sv(cbuild_sv_t* sv, cbuild_sv_t delim) {
	if(delim.size == 0 || delim.size > sv->size) {
		return cbuild_sv_from_parts(sv->data, 0);
	}
	char* chrptr = sv->data - 1;
	size_t i = 0;
	do {
		chrptr = memchr(chrptr + 1, delim.data[0], sv->size - i);
		i = (size_t)(chrptr - sv->data);
		if(chrptr != NULL && sv->size - i >= delim.size &&
			memcmp(chrptr, delim.data, delim.size) == 0) {
			cbuild_sv_t ret = cbuild_sv_from_parts(sv->data, i);
			sv->data += delim.size + i;
			sv->size -= delim.size + i;
			return ret;
		}
	} while(chrptr != NULL);
	return cbuild_sv_chop(sv, sv->size);
}
cbuild_sv_t cbuild_sv_chop_right_by_sv(cbuild_sv_t* sv, cbuild_sv_t delim) {
	if(delim.size == 0 || delim.size > sv->size) {
		return cbuild_sv_from_parts(sv->data, 0);
	}
	char* chrptr = sv->data;
	size_t i = sv->size;
	do {
		chrptr = __cbuild_memrchr(sv->data, delim.data[0], sv->size - (sv->size - i));
		i = (size_t)(chrptr - sv->data);
		if(chrptr != NULL && sv->size - i >= delim.size &&
			memcmp(chrptr, delim.data, delim.size) == 0) {
			cbuild_sv_t ret = cbuild_sv_from_parts(
				sv->data + delim.size + i, 
				sv->size - delim.size - i);
			sv->size = i;
			return ret;
		}
	} while(chrptr != NULL);
	return cbuild_sv_chop(sv, sv->size);
}
cbuild_sv_t cbuild_sv_chop_by_func(cbuild_sv_t* sv, cbuild_sv_delim_func delim,
	void* args) {
	size_t i = 0;
	while(i <= sv->size && !delim(sv, i, args)) {
		i++;
	}
	if(i >= sv->size) {
		return cbuild_sv_chop(sv, i);
	}
	char* tmp = sv->data;
	sv->data += i + 1;
	sv->size -= i + 1;
	return cbuild_sv_from_parts(tmp, i);
}
cbuild_sv_t cbuild_sv_chop_right_by_func(cbuild_sv_t* sv,
	cbuild_sv_delim_func delim, void* args) {
	ssize_t i = (ssize_t)sv->size - 1;
	while(i >= 0 && !delim(sv, (size_t)i, args)) {
		i--;
	}
	if(i < 0) {
		cbuild_sv_t ret = cbuild_sv_from_parts(sv->data, sv->size);
		sv->size = 0;
		return ret;
	}
	size_t tmp = sv->size;
	sv->size = (size_t)i;
	return cbuild_sv_from_parts(sv->data + i + 1, tmp - ((size_t)i + 1));
}
int cbuild_sv_cmp(cbuild_sv_t a, cbuild_sv_t b) {
	if(a.size < b.size) {
		return -2;
	}
	if(a.size > b.size) {
		return 2;
	}
	int ret = memcmp(a.data, b.data, a.size);
	if(ret == 0) {
		return 0;
	} else if(ret < 0) {
		return -1;
	} else if(ret > 0) {
		return 1;
	}
	return 0;
}
int cbuild_sv_cmp_icase(cbuild_sv_t a, cbuild_sv_t b) {
	if(a.size < b.size) {
		return -2;
	}
	if(a.size > b.size) {
		return 2;
	}
	for(size_t i = 0; i < a.size; i++) {
		char ac = 'A' <= a.data[i] && a.data[i] <= 'Z' ? a.data[i] + 32 : a.data[i];
		char bc = 'A' <= b.data[i] && b.data[i] <= 'Z' ? b.data[i] + 32 : b.data[i];
		int diff = ac - bc;
		if(diff < 0) {
			return -1;
		} else if(diff > 0) {
			return 1;
		}
	}
	return 0;
}
bool cbuild_sv_prefix(cbuild_sv_t sv, cbuild_sv_t prefix) {
	if(sv.size < prefix.size) {
		return false;
	}
	return cbuild_sv_cmp(cbuild_sv_from_parts(sv.data, prefix.size), prefix) == 0;
}
bool cbuild_sv_suffix(cbuild_sv_t sv, cbuild_sv_t suffix) {
	if(sv.size < suffix.size) {
		return false;
	}
	return cbuild_sv_cmp(
		cbuild_sv_from_parts(sv.data + sv.size - suffix.size, suffix.size),
		suffix) == 0;
}
ssize_t cbuild_sv_find(cbuild_sv_t sv, char c) {
	if(sv.size == 0) return -1;
	char* chrptr = memchr(sv.data, c, sv.size);
	if(chrptr == NULL) {
		return -1;
	}
	return chrptr - sv.data;
}
ssize_t cbuild_sv_rfind(cbuild_sv_t sv, char c) {
	if(sv.size == 0) return -1;
	char* chrptr = __cbuild_memrchr(sv.data, c, sv.size);
	if(chrptr == NULL) {
		return -1;
	}
	return chrptr - sv.data;
}
ssize_t cbuild_sv_find_sv(cbuild_sv_t sv, cbuild_sv_t needle) {
	char* chrptr = __cbuild_memmem(sv.data, sv.size, needle.data, needle.size);
	if(chrptr == NULL) {
		return -1;
	}
	return chrptr - sv.data;
}
bool cbuild_sv_contains(cbuild_sv_t sv, char c) {
	return cbuild_sv_find(sv, c) != -1;
}
bool cbuild_sv_contains_sv(cbuild_sv_t sv, cbuild_sv_t needle) {
	return cbuild_sv_find_sv(sv, needle) != -1;
}
int cbuild_sv_utf8cp_len(cbuild_sv_t sv) {
	if(sv.size == 0) return 0;
	if((*(unsigned char*)sv.data) < 0x80) return 1;
	if((*sv.data & 0xE0) == 0xC0 && sv.size >= 2) return 2;
	if((*sv.data & 0xF0) == 0xE0 && sv.size >= 3) return 3;
	if((*sv.data & 0xF8) == 0xF0 && sv.size >= 4) return 4;
	return 1;
}
char* cbuild_sv_strchr(cbuild_sv_t sv, char c) {
	return memchr(sv.data, c, sv.size);
}
char* cbuild_sv_utf8chr(cbuild_sv_t sv, uint32_t c) {
	while(sv.size > 0) {
		char* curr = sv.data;
		uint32_t codepoint = cbuild_sv_chop_utf8(&sv);
		if(codepoint == c) return curr;
	}
	return NULL;
}
uint32_t cbuild_sv_chop_utf8(cbuild_sv_t* sv) {
	if(sv->size == 0) return UINT32_MAX;
	unsigned char* s = (unsigned char*)sv->data;
	if(*s < 0x80) {
		sv->data++; s++;
		sv->size--;
		unsigned char byte1 = *(s - 1);
		uint32_t codepoint = byte1;
		return codepoint;
	}
	if((*s & 0xE0) == 0xC0 && sv->size >= 2) {
		sv->data += 2; s += 2;
		sv->size -= 2;
		unsigned char byte1 = *(s - 2),
		byte2 = *(s - 1);
		uint32_t codepoint = ((byte1 & 0x1Fu) << 6) |
			(byte2 & 0x3Fu);
		return codepoint;
	}
	if((*s & 0xF0) == 0xE0 && sv->size >= 3) {
		sv->data += 3; s += 3;
		sv->size -= 3;
		unsigned char byte1 = *(s - 3),
		byte2 = *(s - 2),
		byte3 = *(s - 1);
		uint32_t codepoint = ((byte1 & 0x0Fu) << 12) |
			((byte2 & 0x3Fu) << 6) |
			(byte3 & 0x3Fu);
		return codepoint;
	}
	if((*s & 0xF8) == 0xF0 && sv->size >= 4) {
		sv->data += 4; s += 4;
		sv->size -= 4;
		unsigned char byte1 = *(s - 4),
		byte2 = *(s - 3),
		byte3 = *(s - 2),
		byte4 = *(s - 1);
		uint32_t codepoint = ((byte1 & 0x07u) << 18) |
			((byte2 & 0x3Fu) << 12) |
			((byte3 & 0x3Fu) << 6) |
			(byte4 & 0x3Fu);
		return codepoint;
	}
	sv->data++; s++;
	sv->size--;
	return *(s - 1);
}
cbuild_sv_t cbuild_sv_chop_by_utf8(cbuild_sv_t* sv, uint32_t delim) {
	char* chrptr = cbuild_sv_utf8chr(*sv, delim);
	if(chrptr != NULL) {
		size_t i = (size_t)(chrptr - sv->data);
		cbuild_sv_t ret = cbuild_sv_from_parts(sv->data, i);
		sv->data += i;
		sv->size -= i;
		int codepoint_size = cbuild_sv_utf8cp_len(*sv);
		sv->data += (size_t)codepoint_size;
		sv->size -= (size_t)codepoint_size;
		return ret;
	}
	return cbuild_sv_chop(sv, sv->size);
}
cbuild_sv_t cbuild_sv_chop_by_func_utf8(cbuild_sv_t* sv,
	cbuild_sv_utf8delim_func delim, void* args) {
	cbuild_sv_t ret = cbuild_sv_from_parts(sv->data, 0);
	while(sv->size > 0) {
		size_t cplen = (size_t)cbuild_sv_utf8cp_len(*sv);
		if(delim(sv, args)) {
			sv->data += cplen;
			sv->size -= cplen;
			break;
		} else {
			sv->data += cplen;
			sv->size -= cplen;
			ret.size += cplen;
		}
	}
	return ret;
}
size_t cbuild_sv_utf8len(cbuild_sv_t sv) {
	size_t ret = 0;
	while(sv.size > 0) {
		cbuild_sv_chop_utf8(&sv);
		ret++;
	}
	return ret;
}
bool cbuild_sv_utf8valid(cbuild_sv_t sv, size_t* idx) {
	size_t ret = 0;
	while(sv.size > 0) {
		signed char cs = *(signed char*)sv.data;
		ret++;
		if(cs > 0) { // ASCII, 'signed char' abuse
			sv.size--;
			sv.data++;
			continue;
		}
		unsigned char c = (unsigned char)cs;
		if(0xC2 <= c && c <= 0xDF) { // 2 byte
			sv.size--;
			sv.data++;
			if(sv.size == 0) goto invalid;
			unsigned char c1 = *(unsigned char*)sv.data;
			if(0x80 <= c1 && c1 <= 0xBF) {
				sv.size--;
				sv.data++;
			} else {
				goto invalid;
			}
		} else if(c == 0xE0) { // 3 byte
			sv.size--;
			sv.data++;
			if(sv.size == 0) goto invalid;
			unsigned char c1 = *(unsigned char*)sv.data;
			if(0xA0 <= c1 && c1 <= 0xBF) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c2 = *(unsigned char*)sv.data;
				if(0x80 <= c2 && c2 <= 0xBF) {
					sv.size--;
					sv.data++;
				} else {
					goto invalid;
				}
			} else {
				goto invalid;
			}
		} else if(0xE1 <= c && c <= 0xEC) {
			sv.size--;
			sv.data++;
			if(sv.size == 0) goto invalid;
			unsigned char c1 = *(unsigned char*)sv.data;
			if(0x80 <= c1 && c1 <= 0xBF) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c2 = *(unsigned char*)sv.data;
				if(0x80 <= c2 && c2 <= 0xBF) {
					sv.size--;
					sv.data++;
				} else {
					goto invalid;
				}
			} else {
				goto invalid;
			}
		} else if(c == 0xED) {
			sv.size--;
			sv.data++;
			if(sv.size == 0) goto invalid;
			unsigned char c1 = *(unsigned char*)sv.data;
			if(0x80 <= c1 && c1 <= 0x9F) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c2 = *(unsigned char*)sv.data;
				if(0x80 <= c2 && c2 <= 0xBF) {
					sv.size--;
					sv.data++;
				} else {
					goto invalid;
				}
			} else {
				goto invalid;
			}
		} else if(0xEE <= c && c <= 0xEF) {
			sv.size--;
			sv.data++;
			if(sv.size == 0) goto invalid;
			unsigned char c1 = *(unsigned char*)sv.data;
			if(0x80 <= c1 && c1 <= 0xBF) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c2 = *(unsigned char*)sv.data;
				if(0x80 <= c2 && c2 <= 0xBF) {
					sv.size--;
					sv.data++;
				} else {
					goto invalid;
				}
			} else {
				goto invalid;
			}
		} else if(c == 0xF0) { // 4 byte
			sv.size--;
			sv.data++;
			if(sv.size == 0) goto invalid;
			unsigned char c1 = *(unsigned char*)sv.data;
			if(0x90 <= c1 && c1 <= 0xBF) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c2 = *(unsigned char*)sv.data;
				if(0x80 <= c2 && c2 <= 0xBF) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c3 = *(unsigned char*)sv.data;
					if(0x80 <= c3 && c3 <= 0xBF) {
						sv.size--;
						sv.data++;
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else {
				goto invalid;
			}
		} else if(0xF1 <= c && c <= 0xF3) {
			sv.size--;
			sv.data++;
			if(sv.size == 0) goto invalid;
			unsigned char c1 = *(unsigned char*)sv.data;
			if(0x80 <= c1 && c1 <= 0xBF) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c2 = *(unsigned char*)sv.data;
				if(0x80 <= c2 && c2 <= 0xBF) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c3 = *(unsigned char*)sv.data;
					if(0x80 <= c3 && c3 <= 0xBF) {
						sv.size--;
						sv.data++;
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else {
				goto invalid;
			}
		} else if(c == 0xF4) {
			sv.size--;
			sv.data++;
			if(sv.size == 0) goto invalid;
			unsigned char c1 = *(unsigned char*)sv.data;
			if(0x80 <= c1 && c1 <= 0x8F) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c2 = *(unsigned char*)sv.data;
				if(0x80 <= c2 && c2 <= 0xBF) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c3 = *(unsigned char*)sv.data;
					if(0x80 <= c3 && c3 <= 0xBF) {
						sv.size--;
						sv.data++;
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else {
				goto invalid;
			}
		} else {
			goto invalid;
		}
	}
	return true;
invalid:
	if(idx != NULL) *idx = ret;
	return false;
}
char* cbuild_sv_to_cstr(cbuild_sv_t sv) {
	char* ret = __CBUILD_MALLOC(sv.size + 1);
	cbuild_assert(ret != NULL, "Allocation failed.\n");
	memcpy(ret, sv.data, sv.size);
	ret[sv.size] = 0;
	return ret;
}
char* cbuild_sv_to_temp_cstr(cbuild_sv_t sv) {
	char* ret = cbuild_temp_malloc(sv.size + 1);
	cbuild_assert(ret != NULL, "Allocation failed.\n");
	memcpy(ret, sv.data, sv.size);
	ret[sv.size] = 0;
	return ret;
}
