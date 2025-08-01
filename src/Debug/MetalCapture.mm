// MetalCapture.mm
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>

extern "C" {
    void startMetalCapture(const char* outputPath);
    void stopMetalCapture();
}

static id<MTLCaptureScope> captureScope = nil;

void startMetalCapture(const char* outputPath) {
    @autoreleasepool {
        MTLCaptureManager* captureManager = [MTLCaptureManager sharedCaptureManager];

        // Get default Metal device
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) {
            NSLog(@"Failed to create Metal device");
            return;
        }

        // Create capture descriptor
        MTLCaptureDescriptor* captureDescriptor = [[MTLCaptureDescriptor alloc] init];
        captureDescriptor.captureObject = device;
        captureDescriptor.destination = MTLCaptureDestinationGPUTraceDocument;

        NSString* outputURL = [NSString stringWithUTF8String:outputPath];
        captureDescriptor.outputURL = [NSURL fileURLWithPath:outputURL];

        // Start capture
        NSError* error = nil;
        BOOL success = [captureManager startCaptureWithDescriptor:captureDescriptor error:&error];

        if (!success) {
            NSLog(@"Failed to start Metal capture: %@", error.localizedDescription);
        } else {
            NSLog(@"Started Metal capture to: %s", outputPath);
        }
    }
}

void stopMetalCapture() {
    @autoreleasepool {
        MTLCaptureManager* captureManager = [MTLCaptureManager sharedCaptureManager];
        [captureManager stopCapture];
        NSLog(@"Stopped Metal capture");
    }
}