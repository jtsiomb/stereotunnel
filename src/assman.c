#include <stdio.h>
#include "assman.h"

int ass_fgetc(ass_file *fp)
{
	unsigned char c;

	if(ass_fread(&c, 1, 1, fp) < 1) {
		return -1;
	}
	return (int)c;
}

char *ass_fgets(char *s, int size, ass_file *fp)
{
	int i, c;
	char *ptr = s;

	if(!size) return 0;

	for(i=0; i<size - 1; i++) {
		if((c = ass_fgetc(fp)) == -1) {
			break;
		}
		*ptr++ = c;

		if(c == '\n') break;
	}
	*ptr = 0;
	return ptr == s ? 0 : s;
}
