#ifndef CONFIG_H_
#define CONFIG_H_

#ifdef __APPLE__
#include <TargetConditionals.h>

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#define IPHONE
#define TARGET_IPHONE
#endif

#endif	/* __APPLE__ */

#endif	/* CONFIG_H_ */
