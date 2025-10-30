int main(void) {
	// Validation
	cbuild_sv_t sv1 = cbuild_sv_from_cstr("Привіт, world!");
	TEST_ASSERT(cbuild_sv_utf8valid(sv1, NULL),
		"String view \""CBuildSVFmt"\" should be valid!", CBuildSVArg(sv1));
	unsigned char invalid_utf8_truncated[5] = {0x73, 0xED, 0x83, 0xA0, 0xED};
	cbuild_sv_t sv2 = cbuild_sv_from_parts(invalid_utf8_truncated,
		sizeof(invalid_utf8_truncated));
	TEST_ASSERT(!cbuild_sv_utf8valid(sv2, NULL), "%s",
		"String view with truncated codepoint should be invalid!");
	unsigned char invalid_utf8_prefix[3] = {0x73, 0xC1, 0x85};
	cbuild_sv_t sv3 = cbuild_sv_from_parts(invalid_utf8_prefix,
		sizeof(invalid_utf8_prefix));
	TEST_ASSERT(!cbuild_sv_utf8valid(sv3, NULL), "%s",
		"String view with invalid prefix character should be invalid!");
	unsigned char invalid_utf8_char[7] = {0xED, 0x90, 0x80, 0xF2, 0x97, 0xCF, 0xAC};
	cbuild_sv_t sv4 = cbuild_sv_from_parts(invalid_utf8_char,
		sizeof(invalid_utf8_char));
	TEST_ASSERT(!cbuild_sv_utf8valid(sv4, NULL), "%s",
		"String view with invalid character in codepoint should be invalid!");
	return 0;
}
