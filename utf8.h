#ifndef __UTF8_H_INCLUDED
#define __UTF8_H_INCLUDED
#include <stdint.h>
#include <stddef.h>
typedef uint16_t codepoint_t;
class utf8
{
public:
	static const char *to_codepoint(const char *p, codepoint_t *dst = NULL);
	static int from_codepoint(codepoint_t cp, char *out = NULL);
	static size_t len(const char *p);
};

#endif
