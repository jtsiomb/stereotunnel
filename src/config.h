#ifndef CONFIG_H_
#define CONFIG_H_

#ifdef __APPLE__
#include <TargetConditionals.h>

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#define IPHONE
#endif

#endif	/* __APPLE__ */

#endif	/* CONFIG_H_ */
