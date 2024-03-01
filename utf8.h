#ifndef __UTF8_H_INCLUDED
#define __UTF8_H_INCLUDED
#include <stdint.h>
#include <stddef.h>
typedef uint32_t codepoint_t;
class utf8
{
public:
	static const char *to_codepoint(const char *p, codepoint_t *dst = NULL);
	static int from_codepoint(codepoint_t cp, char *out = NULL);
	static size_t len(const char *p);
	static int prev_line(codepoint_t *buf, int off);
	static int start_line(codepoint_t *buf, int off);
	static bool fmt_next(codepoint_t *buf, int *off, int len, int w, int *x, int *y);
	static int fmt_up(codepoint_t *buf, int off, int w);
	static int fmt_down(codepoint_t *buf, int off, int len, int w);
};

#endif
