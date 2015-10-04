#include <assert.h>
#include <fcntl.h>
#include <android/asset_manager.h>
#include "assman.h"
#include "android_native_app_glue.h"

struct android_app *app;	/* defined in android/amain.c */

ass_file *ass_fopen(const char *fname, const char *mode)
{
	AAsset *ass;
	unsigned int flags = 0;
	char prev = 0;

	while(*mode) {
		switch(*mode) {
		case 'r':
			flags |= O_RDONLY;
			break;

		case 'w':
			flags |= O_WRONLY;
			break;

		case 'a':
			flags |= O_APPEND;
			break;

		case '+':
			if(prev == 'w' || prev == 'a') {
				flags |= O_CREAT;
			}
			break;

		default:
			break;
		}
		prev = *mode++;
	}

	assert(app);
	assert(app->activity);
	assert(app->activity->assetManager);
	if(!(ass = AAssetManager_open(app->activity->assetManager, fname, flags))) {
		return 0;
	}
	return (ass_file*)ass;
}

void ass_fclose(ass_file *fp)
{
	AAsset_close((AAsset*)fp);
}

long ass_fseek(ass_file *fp, long offs, int whence)
{
	return AAsset_seek((AAsset*)fp, offs, whence);
}

long ass_ftell(ass_file *fp)
{
	return AAsset_seek((AAsset*)fp, 0, SEEK_SET);
}

size_t ass_fread(void *buf, size_t size, size_t count, ass_file *fp)
{
	return AAsset_read((AAsset*)fp, buf, size * count) / size;
}
