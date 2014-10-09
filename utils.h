#ifndef _UTILS_H
#define _UTILS_H

# define __ERROR_RETURN(__v, __s)					\
if (__v) {											\
	printk(KERN_INFO "%s: Error in %s\n", MODULE_NAME, __s); \
	return(__v); 									\
}

#endif /* _UTILS_H */