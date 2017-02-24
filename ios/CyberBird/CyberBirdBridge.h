//
//  CyberBird.h
//  CyberBird
//
//  Created by masaaki.goshima on 2017/02/23.
//  Copyright © 2017年 goccy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CyberBird/CBStorage.h>

@class CBStorage;

@interface CyberBirdBridge : NSObject

- (void)wake:(NSURL *)filePath;
- (void)sleep;
- (CBStorage *)storage;

- (NSArray *)fly:(NSString *)tableName latitude:(double)latitude longitude:(double)longitude zoomLevel:(NSUInteger)zoomLevel;

@end
