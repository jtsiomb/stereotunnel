#include <stdio.h>
#include "assman.h"

ass_file *ass_fopen(const char *fname, const char *mode)
{
	return (ass_file*)fopen(fname, mode);
}

void ass_fclose(ass_file *fp)
{
	fclose((FILE*)fp);
}

long ass_fseek(ass_file *fp, long offs, int whence)
{
	fseek((FILE*)fp, offs, whence);
	return ftell((FILE*)fp);
}

long ass_ftell(ass_file *fp)
{
	return ftell((FILE*)fp);
}

size_t ass_fread(void *buf, size_t size, size_t count, ass_file *fp)
{
	return fread(buf, size, count, (FILE*)fp);
}
