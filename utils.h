#ifndef _UTILS_H
#define _UTILS_H

#define INVALID_IRQ     0

#define BOOL            unsigned char
#define TRUE            1
#define FALSE           0

#define _FAIL(__x__, ...)    "[%s->%s] [FAIL] "   __x__ "\n", MODULE_NAME, __FUNCTION__, ##__VA_ARGS__
#define _WARN(__x__, ...)    "[%s->%s] [WARN] "   __x__ "\n", MODULE_NAME, __FUNCTION__, ##__VA_ARGS__
#define _SUCC(__x__, ...)    "[%s->%s] [SUCC] "   __x__ "\n", MODULE_NAME, __FUNCTION__, ##__VA_ARGS__
#define _N(__x__, ...)       "[%s->%s] "          __x__ "\n", MODULE_NAME, __FUNCTION__, ##__VA_ARGS__

#define TRACE_N(__x__, ...)     TRACE(_N(__x__, ##__VA_ARGS__))
#define TRACE_F(__x__, ...)     TRACE(_FAIL(__x__,  ##__VA_ARGS__))
#define TRACE_W(__x__, ...)     TRACE(_WARN(__x__,  ##__VA_ARGS__))
#define TRACE_S(__x__, ...)     TRACE(_SUCC(__x__,  ##__VA_ARGS__))

#define TRACE(format...)        printk(KERN_INFO format);

#endif /* _UTILS_H */