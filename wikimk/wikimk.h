// API exposed by wikimk to plugins, mainly to codeparse
// License: `GPL-3.0-or-later`.
#include "../cbuild.split.h"
bool cbuild_sv_chop_prefix(cbuild_sv_t* sv, cbuild_sv_t prefix);
size_t cbuild_sv_count(cbuild_sv_t sv, char c);
char cbuild_sv_chop_char(cbuild_sv_t* sv);
typedef struct {
	cbuild_sv_t* data;
	size_t size;
	size_t capacity;
} lines_t;
typedef enum {
	DECL_NONE = 0,
	DECL_FUNCTION,
	DECL_VARIABLE,
	DECL_DEFINE,
	DECL_STRUCT,
	DECL_ENUM,
	DECL_TYPEDEF,
	DECL_TYPEDEF_STRUCT,
	DECL_TYPEDEF_ENUM,
	DECL_TYPEDEF_FUNC,
} declaration_type_t; 
typedef struct {
	cbuild_sv_t name;
	cbuild_sv_t type;
} argument_t;
typedef struct {
	argument_t* data;
	size_t size;
	size_t capacity;
} arguments_t;
