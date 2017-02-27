//
//  CBFlightPlan.h
//  Pods
//
//  Created by goccy on 2017/02/27.
//
//

#import <Foundation/Foundation.h>

@interface CBFlightPlan : NSOperation

- (instancetype)initWithPlan:(void(^)(void))plan;

@end
