/****
 The standard assert library uses macros to implements asserts.

 While this is efficient, it is hard to debug.  You cannot set a breakpoint
 in the debugger for standard assert statements.

 This implementation uses a function to implement assert statements

 If you use this implementation you can set break points in the assert function

 ****/

#undef assert

#ifdef NDEBUG           /* required by ANSI standard */
#define assert(p)          ((void)0)
#else
#define assert(e)       ((e) ? (void)0 : __pretty_assert(__FILE__, __LINE__, #e))

#endif /* NDEBUG */

void __pretty_assert(const char *, int, const char *);

