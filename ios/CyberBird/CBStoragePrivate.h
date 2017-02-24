//
//  CBStoragePrivate.h
//  Pods
//
//  Created by masaaki.goshima on 2017/02/24.
//
//

#ifndef CBStoragePrivate_h
#define CBStoragePrivate_h

#include <cyberbird/storage.h>

@interface CBStorage()

@property(nonatomic) cyberbird::Storage *nativeInstance;

- (instancetype)initWithNativeStorage:(cyberbird::Storage *)storage;

@end

#endif /* CBStoragePrivate_h */
