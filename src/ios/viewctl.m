#include <stdlib.h>
#import <OpenGLES/ES2/glext.h>
#import "viewctl.h"
#include "istereo.h"

@interface ViewController () {
	EAGLContext *ctx;
	float pixel_scale, max_pixel_scale, low_pixel_scale;

	GLKView *glview;
	ADBannerView *ad;
	BOOL ad_visible;
	BOOL ad_ready;

	int ios_ver_major, ios_ver_minor;
	BOOL landscape_swap;
}


- (void)create_ad;
- (void)show_ad;
- (void)hide_ad;

- (void)init_gl;
- (void)destroy_gl;

- (void)enable_retina;
- (void)disable_retina;
- (BOOL)is_retina_enabled;
- (BOOL)have_retina;
@end

static ViewController *view_ctl;
static BOOL ad_show_requested;

void ad_banner_show(void)
{
	if(view_ctl) {
		[view_ctl show_ad];
	}
	ad_show_requested = YES;
}

void ad_banner_hide(void)
{
	if(view_ctl) {
		[view_ctl hide_ad];
	}
	ad_show_requested = NO;
}

void use_retina_res(int enable)
{
	if(enable) {
		[view_ctl enable_retina];
	} else {
		[view_ctl disable_retina];
	}
}

int using_retina_res(void)
{
	return [view_ctl is_retina_enabled] ? 1 : 0;
}

int have_retina(void)
{
	if(view_ctl) {
		return [view_ctl have_retina] ? 1 : 0;
	}
	return 0;
}


@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

	view_ctl = self;

	NSString *str = [[UIDevice currentDevice] systemVersion];
	if(sscanf(str.UTF8String, "%d.%d", &ios_ver_major, &ios_ver_minor) != 2) {
		ios_ver_major = 8;
		ios_ver_minor = 0;
	}
	printf("iOS version: %d.%d\n", ios_ver_major, ios_ver_minor);

	// in iOS versions before 8, the view size and event coordinates are swapped in landscape
	if((int)NSFoundationVersionNumber < NSFoundationVersionNumber_iOS_8_0) {
		assert(ios_ver_major < 8);
		landscape_swap = YES;
	} else {
		landscape_swap = NO;
	}

    self->ctx = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2];
    if(!self->ctx) {
        NSLog(@"Failed to create OpenGL ES 2.0 context");
    }

    glview = (GLKView*)self.view;
    glview.context = self->ctx;
    glview.drawableDepthFormat = GLKViewDrawableDepthFormat24;

	if([glview respondsToSelector: NSSelectorFromString(@"contentScaleFactor")]) {
		max_pixel_scale = [[UIScreen mainScreen] scale];
		printf("max pixel scale: %g\n", pixel_scale);
	} else {
		max_pixel_scale = 1.0f;
	}
	low_pixel_scale = 1.0f;
	[self disable_retina];	// default to non-retina mode

	[self create_ad];

	[self init_gl];
}

- (void)dealloc
{
	view_ctl = nil;

	[self destroy_gl];

    if([EAGLContext currentContext] == self->ctx) {
        [EAGLContext setCurrentContext: nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;

        [self destroy_gl];

        if([EAGLContext currentContext] == self->ctx) {
            [EAGLContext setCurrentContext: nil];
        }
        self->ctx = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

- (BOOL)shouldAutorotate
{
	return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
	return UIInterfaceOrientationMaskLandscape;
	//return UIInterfaceOrientationMaskAll;
}

- (void)create_ad
{
	ad = [[ADBannerView alloc] initWithAdType: ADAdTypeBanner];
	CGRect rect = ad.frame;
	rect.size.width = glview.bounds.size.width;
	ad.frame = rect;
	[ad setAutoresizingMask: UIViewAutoresizingFlexibleWidth];

	ad_visible = YES;
	ad_ready = NO;
	[self hide_ad];
	[glview addSubview: ad];
	ad.delegate = self;
}

- (void)show_ad
{
	if(!ad_visible && ad_ready) {
		CGRect rect = ad.frame;
		rect.origin.y = 0;
		ad.frame = rect;
		ad_visible = YES;
	}
}

- (void)hide_ad
{
	if(ad_visible) {
		ad.frame = CGRectOffset(ad.frame, 0, -ad.frame.size.height);
		ad_visible = NO;
	}
}

- (void)init_gl
{
    [EAGLContext setCurrentContext: self->ctx];

	if(init() == -1) {
		NSLog(@"app initialization failed");
		exit(0);
	}
}

- (void)destroy_gl
{
	cleanup();
    [EAGLContext setCurrentContext: self->ctx];
}

- (void)enable_retina
{
	pixel_scale = max_pixel_scale;
	printf("enable_retina setting pixel scale: %g\n", pixel_scale);
	glview.contentScaleFactor = pixel_scale;
	[glview setNeedsLayout];
}

- (void)disable_retina
{
	pixel_scale = low_pixel_scale;
	printf("disable_retina setting pixel scale: %g\n", pixel_scale);
	glview.contentScaleFactor = pixel_scale;
	[glview setNeedsLayout];
}

- (BOOL)is_retina_enabled
{
	return pixel_scale > low_pixel_scale;
}

- (BOOL)have_retina
{
	return max_pixel_scale > 1.0;
}


- (void)glkView: (GLKView*)view drawInRect: (CGRect)rect
{
	redraw();
}

- (void)viewDidLayoutSubviews
{
	CGRect rect = self.view.frame;
	int xsz = rect.size.width * pixel_scale;
	int ysz = rect.size.height * pixel_scale;

	BOOL is_landscape = UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation);
	if(is_landscape && landscape_swap) {
		int tmp = xsz;
		xsz = ysz;
		ysz	= tmp;
	}

	reshape(xsz, ysz);
	printf("viewport %dx%d (scale: %g)\n", xsz, ysz, pixel_scale);
}

- (void)touchesBegan: (NSSet<UITouch*>*)touches withEvent: (UIEvent*)event
{
	UITouch *touch = [touches anyObject];
	CGPoint pt = [touch locationInView: glview];
	int x = (int)(pt.x * pixel_scale);
	int y = (int)(pt.y * pixel_scale);

	mouse_button(0, 1, x, y);
}

- (void)touchesMoved: (NSSet<UITouch*>*)touches withEvent: (UIEvent*)event
{
	UITouch *touch = [touches anyObject];
	CGPoint pt = [touch locationInView: glview];
	int x = (int)(pt.x * pixel_scale);
	int y = (int)(pt.y * pixel_scale);

	mouse_motion(x, y);
}

- (void)touchesEnded: (NSSet<UITouch*>*)touches withEvent: (UIEvent*)event
{
	UITouch *touch = [touches anyObject];
	CGPoint pt = [touch locationInView: glview];
	int x = (int)(pt.x * pixel_scale);
	int y = (int)(pt.y * pixel_scale);

	mouse_button(0, 0, x, y);
	printf("touch release %d %d\n", x, y);
}

- (void)touchesCancelled: (NSSet<UITouch*>*)touches withEvent: (UIEvent*)event
{
	UITouch *touch = [touches anyObject];
	CGPoint pt = [touch locationInView: glview];
	int x = (int)(pt.x * pixel_scale);
	int y = (int)(pt.y * pixel_scale);

	mouse_button(0, 0, x, y);
}

// ADBannerDelegate functions

- (void)bannerViewDidLoadAd: (ADBannerView*)banner
{
	ad_ready = YES;
	if(ad_show_requested) {
		[self show_ad];
	}
}

- (void)bannerView: (ADBannerView*)banner didFailToReceiveAdWithError: (NSError*)error
{
	ad_ready = NO;
	[self hide_ad];

	NSLog(@"Failed to retrieve ad");
}


@end	/* implementation ViewController */
