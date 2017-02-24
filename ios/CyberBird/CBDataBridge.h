//
//  CBDataBridge.h
//  Pods
//
//  Created by masaaki.goshima on 2017/02/24.
//
//

#import <Foundation/Foundation.h>
#import <cyberbird/data.h>

@interface CBDataBridge : NSObject

+ (NSArray *)toNSArray:(cyberbird::array)array;
+ (NSDictionary *)toNSDictionary:(cyberbird::object)object;
+ (cyberbird::array)toNativeArray:(NSArray *)array;
+ (cyberbird::object)toNativeObject:(NSDictionary *)object;

@end
