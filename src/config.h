#ifndef CONFIG_H_
#define CONFIG_H_

#if defined(__IPHONE_3_0) || defined(__IPHONE_3_2) || defined(__IPHONE_4_0)
#define IPHONE

#define glClearDepth	glClearDepthf
#endif

#endif	/* CONFIG_H_ */
