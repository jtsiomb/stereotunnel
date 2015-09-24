#ifndef ASSMAN_H_
#define ASSMAN_H_

#include <stdlib.h>

typedef void ass_file;

#ifdef __cplusplus
extern "C" {
#endif

ass_file *ass_fopen(const char *fname, const char *mode);
void ass_fclose(ass_file *fp);
long ass_fseek(ass_file *fp, long offs, int whence);
long ass_ftell(ass_file *fp);

size_t ass_fread(void *buf, size_t size, size_t count, ass_file *fp);

/* convenience functions, derived from the above */
int ass_fgetc(ass_file *fp);
char *ass_fgets(char *s, int size, ass_file *fp);

#ifdef __cplusplus
}
#endif

#endif	/* ASSMAN_H_ */
