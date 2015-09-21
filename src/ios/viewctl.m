#include <stdlib.h>
#import <OpenGLES/ES2/glext.h>
#import "viewctl.h"
#include "istereo.h"

@interface ViewController () {
	EAGLContext *ctx;
	float pixel_scale;

	ADBannerView *ad;
	BOOL ad_visible;
}


- (void)create_ad;
- (void)show_ad;
- (void)hide_ad;

- (void)init_gl;
- (void)destroy_gl;
@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];


    self->ctx = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2];
    if(!self->ctx) {
        NSLog(@"Failed to create OpenGL ES 2.0 context");
    }

    GLKView *view = (GLKView*)self.view;
    view.context = self->ctx;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

	if([view respondsToSelector: NSSelectorFromString(@"contentScaleFactor")]) {
		pixel_scale = [[UIScreen mainScreen] scale];
		view.contentScaleFactor = pixel_scale;
		printf("pixel scale: %g\n", pixel_scale);
	} else {
		pixel_scale = 1.0f;
	}

	[self create_ad];

	[self init_gl];
}

- (void)dealloc
{
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

- (void)create_ad
{
	ad = [[ADBannerView alloc] initWithAdType: ADAdTypeBanner];
	[ad setAutoresizingMask: UIViewAutoresizingFlexibleWidth];
	ad_visible = YES;
	[self hide_ad];
	[self.view addSubview: ad];
	ad.delegate = self;
}

- (void)show_ad
{
	if(!ad_visible) {
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


- (void)glkView: (GLKView*)view drawInRect: (CGRect)rect
{
	redraw();
}

- (void)viewDidLayoutSubviews
{
	CGRect rect = self.view.frame;
	int xsz = rect.size.width * pixel_scale;
	int ysz = rect.size.height * pixel_scale;
	reshape(xsz, ysz);
	printf("viewport %dx%d (scale: %g)\n", xsz, ysz, pixel_scale);
}

// ADBannerDelegate functions

- (void)bannerViewDidLoadAd: (ADBannerView*)banner
{
	[self show_ad];
}

- (void)bannerView: (ADBannerView*)banner didFailToReceiveAdWithError: (NSError*)error
{
	[self hide_ad];

	NSLog(@"Failed to retrieve ad");
}


@end	/* implementation ViewController */
