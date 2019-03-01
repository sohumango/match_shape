#import <Cocoa/Cocoa.h>
#import <string.h>
#import <stdio.h>

#define KEYCODE_ESC 53
#define KEYCODE_LEFT 123
#define KEYCODE_RIGHT 124

typedef struct
{
    int width;
    int height;
    int channel;
    unsigned char *pixels;
} ImageForShow;
static ImageForShow * pimg;

NSWindow *window;
NSImageView *image_view;

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end
//-----------------------------------------------------------------------
@implementation AppDelegate

- (void)dealloc
{
    [super dealloc];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app
{
    return YES;
}

- (void)applicationDidFinishLaunching:(NSNotification *)note
{
    NSEvent* (^handler)(NSEvent*) = ^(NSEvent *event)
    {
        NSEvent *result = event;
        if(event.keyCode == KEYCODE_ESC ||
           event.keyCode == 12 ) // or Q key
        {
            [NSApp terminate:nil];
            result = nil;
        }
        else{
            NSLog(@"%d", event.keyCode);
        }
            
        return result;
    };
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown handler:handler];
    [self displayImage:pimg->pixels width:pimg->width height:pimg->height channel:pimg->channel];
}


- (void)displayImage:(unsigned char*)pixels width:(int)w height:(int)h channel:(int)c
{
    [window setFrame:[window frameRectForContentRect:NSMakeRect(0, 0, w, h)] display:YES];
    [window center];

    id old_image = [image_view image];
    if(old_image != nil){ [old_image release]; }

    NSBitmapImageRep *representation;
    BOOL hasA=NO;
    if(c==4) hasA=YES;
    
    if( c>= 3) {
        representation = [[NSBitmapImageRep alloc]
            initWithBitmapDataPlanes: NULL
            pixelsWide: w
            pixelsHigh: h
            bitsPerSample: 8
            samplesPerPixel: c
            hasAlpha: hasA
            isPlanar: NO
            colorSpaceName: NSCalibratedRGBColorSpace
            bytesPerRow: w*c
            bitsPerPixel: 0
        ];
    } else {
        representation = [[NSBitmapImageRep alloc]
                          initWithBitmapDataPlanes: nil
                          pixelsWide: w
                          pixelsHigh: h
                          bitsPerSample: 8
                          samplesPerPixel: c
                          hasAlpha: NO
                          isPlanar: NO
                          colorSpaceName: NSCalibratedWhiteColorSpace
                          bytesPerRow: w*c
                          bitsPerPixel: 0
                          ];
    }
    memcpy([representation bitmapData], pixels, w*h*c);
    
    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(w, h)];
    [image addRepresentation:representation];
    [representation release];
    //
    NSLog(@"Retina display mode is %f",NSScreen.mainScreen.backingScaleFactor);
    //NSImage *a = [[NSImage alloc] initWithContentsOfFile:@"res/bf.png"];   [image_view setImage:a];
    [image_view setImage:image];
}

@end

//---------------------------------------------------------

void rgb2bgr(unsigned char src[], int w, int h, int channel){
    int c = channel;
    int len = w*h*c;
    for (int i=0; i+c<len; i+=c) {
        unsigned char temp = src[i+0];
        src[i+0] = src[i+2];
        src[i+2] = temp;
    }
}

int imshow( unsigned char * buf, int w, int h, int channel, bool bbgr2rgb)
{
    pimg = (ImageForShow*)malloc(sizeof(ImageForShow));
    unsigned char * bufloc = (unsigned char*)malloc(w*h*channel);
    memcpy(bufloc, buf, w*h*channel);
    if( bbgr2rgb ) { rgb2bgr(bufloc,w,h,channel); }
    pimg->pixels = bufloc;
    pimg->width = w;
    pimg->height = h;
    pimg->channel = channel;

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    id app_name = @"imshow";

    AppDelegate *delegate = [AppDelegate new];

    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp setDelegate:delegate];

    NSUInteger window_style = NSWindowStyleMaskTitled |
                                NSWindowStyleMaskClosable |
                                NSWindowStyleMaskResizable |
                                NSWindowStyleMaskMiniaturizable;
    NSRect window_rect = NSMakeRect(10, 10, 640, 480);
    window = [[NSWindow alloc]  initWithContentRect:window_rect
                                styleMask:window_style
                                backing:NSBackingStoreBuffered
                                defer:NO];
    [window autorelease];
    [window setTitle:app_name];

    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(640, 480)];
    image_view = [NSImageView imageViewWithImage:image];
    [image_view autorelease];
    
    [[image_view layer] setOpaque:FALSE];
    image_view.alphaValue = 1.0f;
    
    [image_view setImageScaling:NSImageScaleNone];
    [image_view setImageFrameStyle:NSImageFrameNone];
    [image_view setImageAlignment:NSImageAlignCenter];

    [window setContentView:image_view];

    NSWindowController *win_controller = [[NSWindowController alloc] initWithWindow:window];
    [win_controller autorelease];

    [window makeKeyAndOrderFront:nil];
    [window center];

    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];

    [pool drain];
    
    free(bufloc);
    free(pimg);
    return 0;
}
