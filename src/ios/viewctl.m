#import "viewctl.h"
#import <OpenGLES/ES2/glext.h>

@interface ViewController () {
	EAGLContext *ctx;

	ADBannerView *ad;
	BOOL ad_visible;
}


- (void)create_ad;

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
	ad_visible = NO;
	ad = [[ADBannerView alloc] initWithAdType: ADAdTypeBanner];
	[ad setAutoresizingMask: UIViewAutoresizingFlexibleWidth];
	ad.frame = CGRectOffset(ad.frame, 0, -ad.frame.size.height);
	[self.view addSubview: ad];
	ad.delegate = self;
}

- (void)init_gl
{
    [EAGLContext setCurrentContext: self->ctx];

	glClearColor(1.0, 0.0, 0.0, 1.0);
}

- (void)destroy_gl
{
    [EAGLContext setCurrentContext: self->ctx];
}


- (void)glkView: (GLKView*)view drawInRect: (CGRect)rect
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// ADBannerDelegate functions

- (void)bannerViewDidLoadAd: (ADBannerView*)banner
{
	if(!ad_visible) {
		CGRect rect = ad.frame;
		rect.origin.y = 0;
		ad.frame = rect;
		ad_visible = YES;
	}
}

- (void)bannerView: (ADBannerView*)banner didFailToReceiveAdWithError: (NSError*)error
{
	if(ad_visible) {
		ad_visible = NO;
		ad.frame = CGRectOffset(ad.frame, 0, -ad.frame.size.height);
	}

	NSLog(@"Failed to retrieve ad");
}


@end	/* implementation ViewController */
