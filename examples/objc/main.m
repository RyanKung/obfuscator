#import <Foundation/Foundation.h>


@interface Box:NSObject {
   double length;    // Length of a box
   double breadth;   // Breadth of a box
   double height;    // Height of a box
}

@property(nonatomic, readwrite) double height;  // Property
-(double) volume;
@end

@implementation Box

@synthesize height;

-(id)init {
   self = [super init];
   length = 1.0;
   width = 1.0;
   return self;
}

-(double) volume {
   return length*breadth*height;
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
      double volume;
      NSLog(@"Hello, World!");
      Box *box = [[Box alloc] init];
      volume = [box volume];
      NSLog(@"Volume of Box : %f", volume);
    }
    return 0;
}
