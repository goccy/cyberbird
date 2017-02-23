//
//  CyberBird.m
//  CyberBird
//
//  Created by masaaki.goshima on 2017/02/23.
//  Copyright © 2017年 goccy. All rights reserved.
//

#import "CyberBirdBridge.h"
#import "CBStorage.h"
#import <cyberbird/cyberbird.h>

@interface CBStorage()

@property(nonatomic) cyberbird::Storage *nativeInstance;

@end

@implementation CBStorage

- (instancetype)initWithNativeStorage:(cyberbird::Storage *)storage
{
    self = [super init];
    self.nativeInstance = storage;
    return self;
}

@end

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

- (oneway void)release
{
    CYBER_BIRD_SAFE_DELETE(self.nativeInstance);
}

- (void)wake:(NSURL *)filePath
{
    self.nativeInstance->wake([[filePath path] UTF8String]);
}

- (CBStorage *)storage
{
    return [[CBStorage alloc] initWithNativeStorage:self.nativeInstance->storage()];
}

@end
