#include "utf8.h"

int
utf8::from_codepoint(codepoint_t cp, char *out)
{
	if (cp <= 0x7F) {
		if (out) {
			out[0] = (char) cp;
		}
		return 1;
	} else if (cp <= 0x07FF) {
		// 2-byte unicode
		if (out) {
			out[0] = (char) (((cp >> 6) & 0x1F) | 0xC0);
			out[1] = (char) (((cp >> 0) & 0x3F) | 0x80);
		}
		return 2;
	} else if (cp <= 0xFFFF) {
		// 3-byte unicode
		if (out) {
			out[0] = (char) (((cp >> 12) & 0x0F) | 0xE0);
			out[1] = (char) (((cp >>  6) & 0x3F) | 0x80);
			out[2] = (char) (((cp >>  0) & 0x3F) | 0x80);
		}
		return 3;
	} else if (cp <= 0x10FFFF) {
		// 4-byte unicode
		if (out) {
			out[0] = (char) (((cp >> 18) & 0x07) | 0xF0);
			out[1] = (char) (((cp >> 12) & 0x3F) | 0x80);
			out[2] = (char) (((cp >>  6) & 0x3F) | 0x80);
			out[3] = (char) (((cp >>  0) & 0x3F) | 0x80);
		}
		return 4;
	}
	return -1;
}
const char*
utf8::to_codepoint(const char *p, codepoint_t *dst)
{
	unsigned res, n;
	switch (*p & 0xf0) {
		case 0xf0 :  res = *p & 0x07;  n = 3;  break;
		case 0xe0 :  res = *p & 0x0f;  n = 2;  break;
		case 0xd0 :
		case 0xc0 :  res = *p & 0x1f;  n = 1;  break;
		default   :  res = *p;         n = 0;  break;
	}
	while (n-- && *p)
		res = (res << 6) | (*(++p) & 0x3f);
	if (dst)
		*dst = res;
	return p + 1;
}

size_t
utf8::len(const char *p)
{
	size_t sz = 0;
	while (*p) {
		p = to_codepoint(p);
		sz ++;
	}
	return sz;
}
