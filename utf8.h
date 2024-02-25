#ifndef __UTF8_H_INCLUDED
#define __UTF8_H_INCLUDED
#include <stdint.h>
typedef uint16_t codepoint_t;
class utf8
{
public:
	static const char *to_codepoint(const char *p, codepoint_t *dst);
};

#endif
