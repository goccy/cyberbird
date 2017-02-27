//
//  CBFlightEngine.m
//  Pods
//
//  Created by goccy on 2017/02/27.
//
//

#import "CBFlightEngine.h"

@implementation CBFlightEngine

static const int AVAILABLE_QUEUE_SIZE = 5;

- (void)add:(NSOperation *)queue
{
    if ([self operationCount] > AVAILABLE_QUEUE_SIZE) return;
    NSOperation *lastOperation = [[self operations] lastObject];
    if (lastOperation) [queue addDependency:lastOperation];
    [self addOperation:queue];
}

@end
