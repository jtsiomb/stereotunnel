#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <EGL/egl.h>
#include <jni.h>
#include "android_native_app_glue.h"
#include <android/window.h>
#include <android/sensor.h>
#include "logger.h"
#include "istereo.h"

struct android_app *app;

static void handle_command(struct android_app *app, int32_t cmd);
static int handle_input(struct android_app *app, AInputEvent *ev);
static int handle_touch_input(struct android_app *app, AInputEvent *ev);
static int init_gl(void);
static void destroy_gl(void);

static EGLDisplay dpy;
static EGLSurface surf;
static EGLContext ctx;

static int redisp_pending = 1;	/* TODO stop busy-looping */

static int width, height;
static int init_done;

static JavaVM *jvm;
static JNIEnv *jni;
static jclass activity_class;
static jobject activity;

void android_main(struct android_app *app_ptr)
{
	app_dummy();
	app = app_ptr;

	app->onAppCmd = handle_command;
	app->onInputEvent = handle_input;

	//ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FULLSCREEN, 0);

	start_logger();

	jvm = app->activity->vm;
	if((*jvm)->AttachCurrentThread(jvm, &jni, 0) != 0) {
		fprintf(stderr, "failed to attach native thread to Java VM\n");
		exit(1);
	}
	activity = app->activity->clazz;
	activity_class = (*jni)->GetObjectClass(jni, app->activity->clazz);

	for(;;) {
		int num_events;
		struct android_poll_source *pollsrc;

		while(ALooper_pollAll(0, 0, &num_events, (void**)&pollsrc) >= 0) {
			if(pollsrc) {
				pollsrc->process(app, pollsrc);
			}
		}

		if(app->destroyRequested) {
			return;
		}

		if(init_done && redisp_pending) {
			redraw();
			eglSwapBuffers(dpy, surf);
		}
	}
}

void set_mouse_pos(int x, int y)
{
}

void set_mouse_cursor(int enable)
{
}

void ad_banner_show(void)
{
	jmethodID method;
	if(!jvm) return;

	if(!(method = (*jni)->GetMethodID(jni, activity_class, "show_ad", "()V"))) {
		fprintf(stderr, "failed to retrieve MainActivity.show_ad method\n");
		return;
	}
	(*jni)->CallVoidMethod(jni, activity, method);
}

void ad_banner_hide(void)
{
	jmethodID method;
	if(!jvm) return;

	if(!(method = (*jni)->GetMethodID(jni, activity_class, "hide_ad", "()V"))) {
		fprintf(stderr, "failed to retrieve MainActivity.hide_ad method\n");
		return;
	}
	(*jni)->CallVoidMethod(jni, activity, method);
}

static void handle_command(struct android_app *app, int32_t cmd)
{
	switch(cmd) {
	case APP_CMD_SAVE_STATE:
		/* save the application state to be reloaded on restart if needed */
		break;

	case APP_CMD_INIT_WINDOW:
		printf("APP_CMD_INIT_WINDOW\n");
		ad_banner_show();

		if(init_gl() == -1) {
			exit(1);
		}
		/* initialize the application */
		if(init() == -1) {
			exit(1);	/* initialization failed, quit */
		}
		reshape(width, height);
		init_done = 1;
		break;

	case APP_CMD_TERM_WINDOW:
		/* cleanup */
		printf("APP_CMD_TERM_WINDOW\n");
		init_done = 0;
		cleanup();
		destroy_gl();
		break;

	case APP_CMD_GAINED_FOCUS:
		printf("APP_CMD_GAINED_FOCUS\n");
		/* app focused */
		break;

	case APP_CMD_LOST_FOCUS:
		printf("APP_CMD_LOST_FOCUS\n");
		/* app lost focus */
		break;

	case APP_CMD_WINDOW_RESIZED:
	case APP_CMD_CONFIG_CHANGED:
		{
			int nx = ANativeWindow_getWidth(app->window);
			int ny = ANativeWindow_getHeight(app->window);
			if(nx != width || ny != height) {
				printf("reshape(%d, %d)\n", nx, ny);
				reshape(nx, ny);
				width = nx;
				height = ny;
			}
		}
		break;

	default:
		break;
	}
}

static int handle_input(struct android_app *app, AInputEvent *ev)
{
	int evtype = AInputEvent_getType(ev);

	switch(evtype) {
	case AINPUT_EVENT_TYPE_MOTION:
		return handle_touch_input(app, ev);

	default:
		break;
	}
	return 0;
}

#define MAX_TOUCH_IDS	32

static int handle_touch_input(struct android_app *app, AInputEvent *ev)
{
	int x, y, idx, touch_id;
	unsigned int action;
	static int prev_pos[MAX_TOUCH_IDS][2];

	action = AMotionEvent_getAction(ev);

	idx = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
		AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
	touch_id = AMotionEvent_getPointerId(ev, idx);

	x = AMotionEvent_getX(ev, idx);
	y = AMotionEvent_getY(ev, idx);

	switch(action & AMOTION_EVENT_ACTION_MASK) {
	case AMOTION_EVENT_ACTION_DOWN:
	case AMOTION_EVENT_ACTION_POINTER_DOWN:
		if(touch_id == 0) {
			mouse_button(0, 1, x, y);
		}
		if(touch_id < MAX_TOUCH_IDS) {
			prev_pos[touch_id][0] = x;
			prev_pos[touch_id][1] = y;
		}
		break;

	case AMOTION_EVENT_ACTION_UP:
	case AMOTION_EVENT_ACTION_POINTER_UP:
		if(touch_id == 0) {
			mouse_button(0, 0, x, y);
		}
		if(touch_id < MAX_TOUCH_IDS) {
			prev_pos[touch_id][0] = x;
			prev_pos[touch_id][1] = y;
		}
		break;

	case AMOTION_EVENT_ACTION_MOVE:
		{
			int i, pcount = AMotionEvent_getPointerCount(ev);
			for(i=0; i<pcount; i++) {
				int id = AMotionEvent_getPointerId(ev, i);
				if(id < MAX_TOUCH_IDS && x != prev_pos[id][0] && y != prev_pos[id][1]) {
					if(id == 0) {
						mouse_motion(x, y);
					}
					prev_pos[id][0] = x;
					prev_pos[id][1] = y;
				}
			}
		}
		break;

	default:
		break;
	}

	return 1;
}


static int init_gl(void)
{
	static const int eglattr[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 5,
		EGL_BLUE_SIZE, 5,
		EGL_DEPTH_SIZE, 16,
		EGL_NONE
	};
	static const int ctxattr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

	EGLConfig eglcfg;
	int count, vis;

	dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(!dpy || !eglInitialize(dpy, 0, 0)) {
		fprintf(stderr, "failed to initialize EGL\n");
		destroy_gl();
		return -1;
	}

	if(!eglChooseConfig(dpy, eglattr, &eglcfg, 1, &count)) {
		fprintf(stderr, "no matching EGL config found\n");
		destroy_gl();
		return -1;
	}

	/* configure the native window visual according to the chosen EGL config */
	eglGetConfigAttrib(dpy, &eglcfg, EGL_NATIVE_VISUAL_ID, &vis);
	ANativeWindow_setBuffersGeometry(app->window, 0, 0, vis);

	if(!(surf = eglCreateWindowSurface(dpy, eglcfg, app->window, 0))) {
		fprintf(stderr, "failed to create window\n");
		destroy_gl();
		return -1;
	}

	if(!(ctx = eglCreateContext(dpy, eglcfg, EGL_NO_CONTEXT, ctxattr))) {
		fprintf(stderr, "failed to create OpenGL ES context\n");
		destroy_gl();
		return -1;
	}
	eglMakeCurrent(dpy, surf, surf, ctx);

	eglQuerySurface(dpy, surf, EGL_WIDTH, &width);
	eglQuerySurface(dpy, surf, EGL_HEIGHT, &height);
	printf("initial reshape call: %dx%d\n", width, height);
	reshape(width, height);

	return 0;
}

static void destroy_gl(void)
{
	if(!dpy) return;

	eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	if(ctx) {
		eglDestroyContext(dpy, ctx);
		ctx = 0;
	}
	if(surf) {
		eglDestroySurface(dpy, surf);
		surf = 0;
	}

	eglTerminate(dpy);
	dpy = 0;
}
