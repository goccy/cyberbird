//
//  CBFlightEngine.h
//  Pods
//
//  Created by goccy on 2017/02/27.
//
//

#import <Foundation/Foundation.h>

@interface CBFlightEngine : NSOperationQueue

- (void)add:(NSOperation *)queue;

@end
