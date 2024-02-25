#include "utf8.h"
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
	if (dst) {
		while (n-- && *p)
			res = (res << 6) | (*(++p) & 0x3f);
		*dst = res;
	}
	return p + 1;
}

size_t
utf8::len(const char *p)
{
	const char *eptr = p;
	while ((eptr = to_codepoint(eptr, NULL))[0]);
	return (size_t)(eptr - p);
}