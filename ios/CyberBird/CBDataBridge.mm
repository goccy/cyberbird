//
//  CBDataBridge.m
//  Pods
//
//  Created by masaaki.goshima on 2017/02/24.
//
//

#import "CBDataBridge.h"

@implementation CBDataBridge

+ (NSArray *)toNSArray:(cyberbird::array)array
{
    NSMutableArray *ret = [@[] mutableCopy];
    for (size_t i = 0; i < array.size(); ++i) {
        cyberbird::object object = array[i].get<cyberbird::object>();
        [ret addObject:[CBDataBridge toNSDictionary:object]];
    }
    return ret;
}

+ (NSDictionary *)toNSDictionary:(cyberbird::object)object
{
    NSMutableDictionary *ret = [@{} mutableCopy];
    cyberbird::object::iterator it = object.begin();
    for (; it != object.end(); ++it) {
        std::string key = it->first;
        cyberbird::value value = it->second;
        if (value.is<std::string>()) {
            ret[@(key.c_str())] = @(value.get<std::string>().c_str());
        } else if (value.is<long long int>()) {
            ret[@(key.c_str())] = @(value.get<long long int>());
        }
    }
    return ret;
}

+ (cyberbird::array)toNativeArray:(NSArray *)array
{
    cyberbird::array ret;
    for (NSDictionary *elem in array) {
        ret.push_back(cyberbird::value([CBDataBridge toNativeObject:elem]));
    }
    return ret;
}

+ (cyberbird::object)toNativeObject:(NSDictionary *)object
{
    cyberbird::object ret;
    for (NSString *key in [object allKeys]) {
        id value = object[key];
        if ([value isKindOfClass:[NSString class]]) {
            ret.insert(std::make_pair(std::string([key UTF8String]), cyberbird::value(std::string([value UTF8String]))));
        } else if ([value isKindOfClass:[NSNumber class]]) {
            ret.insert(std::make_pair(std::string([key UTF8String]), cyberbird::value([value intValue])));
        }
    }
    return ret;
}

@end
