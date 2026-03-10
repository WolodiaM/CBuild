#include "StringBuilder.h"
#include "StringView.h"
#include "Temp.h"
#include "DynArray.h"
CBUILDDEF int cbuild_sb_cmp(cbuild_sb_t a, cbuild_sb_t b) {
	return cbuild_sv_cmp(cbuild_sb_to_sv(a), cbuild_sb_to_sv(b));
}
CBUILDDEF int cbuild_sb_cmp_icase(cbuild_sb_t a, cbuild_sb_t b) {
	return cbuild_sv_cmp_icase(cbuild_sb_to_sv(a), cbuild_sb_to_sv(b));
}
CBUILDDEF void cbuild_sb_append_utf8(cbuild_sb_t* sb, uint32_t cp) {
	unsigned char buffer[4] = {0};
	size_t len = 0;
	if(cp <= 0x7F) {
		buffer[0] = (unsigned char)cp;
		len = 1;
	} else if(cp <= 0x7FF) {
		buffer[0] = 0xC0 | ((cp >> 6) & 0x1F);
		buffer[1] = 0x80 | (cp & 0x3F);
		len = 2;
	} else if(cp <= 0xFFFF) {
		buffer[0] = 0xE0 | ((cp >> 12) & 0x0F);
		buffer[1] = 0x80 | ((cp >> 6) & 0x3F);
		buffer[2] = 0x80 | (cp & 0x3F);
		len = 3;
	} else if(cp <= 0x10FFFF) {
		buffer[0] = 0xF0 | ((cp >> 18) & 0x07);
		buffer[1] = 0x80 | ((cp >> 12) & 0x3F);
		buffer[2] = 0x80 | ((cp >> 6) & 0x3F);
		buffer[3] = 0x80 | (cp & 0x3F);
		len = 4;
	} else {
		CBUILD_UNREACHABLE("Invalid Unicode codepoint found: 0x%04X\n", cp);
	}
	cbuild_sb_append_arr(sb, buffer, len);
	
}
CBUILDDEF size_t cbuild_sb_utf8len(cbuild_sb_t sb) {
	return cbuild_sv_utf8len(cbuild_sb_to_sv(sb));
}
CBUILDDEF cbuild_sv_t cbuild_sb_to_sv(cbuild_sb_t sb) {
	return (cbuild_sv_t) {
		.data = sb.data, .size = sb.size
	};
}
CBUILDDEF cbuild_sb_t cbuild_sv_to_sb(cbuild_sv_t sv) {
	cbuild_sb_t ret = {0};
	cbuild_sb_append_arr(&ret, sv.data, sv.size);
	return ret;
}
CBUILDDEF int cbuild_sb_appendf(cbuild_sb_t* sb, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = cbuild_sb_vappendf(sb, fmt, args);
	va_end(args);
	return ret;
}
CBUILDDEF int cbuild_sb_vappendf(cbuild_sb_t* sb, const char* fmt, va_list args) {
	va_list args_copy;
	va_copy(args_copy, args);
	char buff[CBUILD_QUICK_SPRINTF_SIZE];
	int ret = vsnprintf(buff, CBUILD_QUICK_SPRINTF_SIZE, fmt, args);
	if(ret < 0) {
		va_end(args_copy);
		return ret;
	}
	if((size_t)ret >= CBUILD_QUICK_SPRINTF_SIZE) {
		char* buff1 = __CBUILD_MALLOC((size_t)ret + 1);
		cbuild_assert(buff1 != NULL, "Allocation failed.\n");
		vsnprintf(buff1, (size_t)ret + 1, fmt, args_copy);
		cbuild_sb_append_arr(sb, buff1, (size_t)ret);
		__CBUILD_FREE(buff1);
	} else {
		cbuild_sb_append_arr(sb, buff, (size_t)ret);
	}
	va_end(args_copy);
	return ret;
}
