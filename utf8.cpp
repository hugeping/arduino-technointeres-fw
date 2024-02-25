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
	while (n-- && *p)
		res = (res << 6) | (*(++p) & 0x3f);
	*dst = res;
	return p + 1;
}
