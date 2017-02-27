//
//  CBFlightPlan.m
//  Pods
//
//  Created by goccy on 2017/02/27.
//
//

#import "CBFlightPlan.h"

@interface CBFlightPlan()

@property(nonatomic) void(^plan)(void);

@end

@implementation CBFlightPlan

- (instancetype)initWithPlan:(void(^)(void))plan
{
    self = [super init];
    self.plan = plan;
    return self;
}

- (void)main
{
    self.plan();
}

@end
