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

@interface CyberBirdBridge()

@property(nonatomic) cyberbird::CyberBird *nativeInstance;

@end

@implementation CyberBirdBridge

- (instancetype)init
{
    self = [super init];
    self.nativeInstance = new cyberbird::CyberBird();
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

- (NSArray *)fly:(NSString *)tableName latitude:(double)latitude longitude:(double)longitude zoomLevel:(NSUInteger)zoomLevel
{
    cyberbird::array array = self.nativeInstance->fly([tableName UTF8String], latitude, longitude, zoomLevel);
    return [CBDataBridge toNSArray:array];
}

- (CBStorage *)storage
{
    return [[CBStorage alloc] initWithNativeStorage:self.nativeInstance->storage()];
}

@end
