#import "viewctl.h"
#import <OpenGLES/ES2/glext.h>

@interface ViewController () {
	EAGLContext *ctx;
}

- (void)initgl;
- (void)destroygl;
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

	[self initgl];
}

- (void)dealloc
{
	[self destroygl];

    if([EAGLContext currentContext] == self->ctx) {
        [EAGLContext setCurrentContext: nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;

        [self destroygl];

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

- (void)initgl
{
    [EAGLContext setCurrentContext: self->ctx];

	glClearColor(1.0, 0.0, 0.0, 1.0);
}

- (void)destroygl
{
    [EAGLContext setCurrentContext: self->ctx];
}

- (void)update
{
}

- (void)glkView: (GLKView*)view drawInRect: (CGRect)rect
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

@end	/* implementation ViewController */
