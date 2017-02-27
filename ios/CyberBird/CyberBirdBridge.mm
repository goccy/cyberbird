//
//  CyberBird.m
//  CyberBird
//
//  Created by masaaki.goshima on 2017/02/23.
//  Copyright © 2017年 goccy. All rights reserved.
//

#import "CyberBirdBridge.h"
#import "CBStorage.h"
#import "CBDataBridge.h"
#import <cyberbird/cyberbird.h>
#import "CBStoragePrivate.h"
#import "CBFlightEngine.h"
#import "CBFlightPlan.h"

@interface CyberBirdBridge()

@property(nonatomic) cyberbird::CyberBird *nativeInstance;
@property(nonatomic) CBFlightEngine *flightEngine;

@end

@implementation CyberBirdBridge

- (instancetype)init
{
    self = [super init];
    self.nativeInstance = new cyberbird::CyberBird();
    self.flightEngine   = [CBFlightEngine new];
    return self;
}

- (void)wake:(NSURL *)filePath
{
    self.nativeInstance->wake([[filePath path] UTF8String]);
}

- (void)sleep
{
    self.nativeInstance->sleep();
}

- (void)fly:(NSString *)tableName latitude:(double)latitude longitude:(double)longitude zoomLevel:(NSUInteger)zoomLevel didCompletion:(void(^)(NSArray *foundLocations))didCompletion
{
    CBFlightPlan *plan = [[CBFlightPlan alloc] initWithPlan:^{
        cyberbird::array array = self.nativeInstance->fly([tableName UTF8String], latitude, longitude, zoomLevel);
        NSArray *locations = [CBDataBridge toNSArray:array];
        dispatch_async(dispatch_get_main_queue(), ^{
            didCompletion(locations);
        });
    }];
    [self.flightEngine add:plan];
}

- (CBStorage *)storage
{
    return [[CBStorage alloc] initWithNativeStorage:self.nativeInstance->storage()];
}

@end
