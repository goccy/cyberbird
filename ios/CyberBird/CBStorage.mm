//
//  CBStorage.m
//  Pods
//
//  Created by masaaki.goshima on 2017/02/23.
//
//

#import "CBStorage.h"
#import "CBStoragePrivate.h"
#import "CBDataBridge.h"

@interface CBTableBuilder()

@property(nonatomic) cyberbird::Table::Builder *nativeInstance;
@property(nonatomic, readwrite) NSString *tableName;

@end

@implementation CBTableBuilder

- (instancetype)initWithTableName:(NSString *)tableName
{
    self = [super init];
    self.tableName = tableName;
    self.nativeInstance = new cyberbird::Table::Builder([tableName UTF8String]);
    return self;
}

- (instancetype)addNumberColumn:(NSString *)columnName
{
    self.nativeInstance->addNumberColumn([columnName UTF8String]);
    return self;
}

- (instancetype)addStringColumn:(NSString *)columnName size:(NSUInteger)columnSize
{
    self.nativeInstance->addStringColumn([columnName UTF8String], columnSize);
    return self;
}

- (CBTable *)build
{
    return [[CBTable alloc] initWithNativeInstance:self.nativeInstance->build()];
}

@end


@interface CBTable()

@property(nonatomic) cyberbird::Table *nativeInstance;
@property(nonatomic, readwrite) NSString *tableName;

- (instancetype)initWithNativeInstance:(cyberbird::Table *)table;

@end

@implementation CBTable

- (instancetype)initWithNativeInstance:(cyberbird::Table *)table
{
    self = [super init];
    self.nativeInstance = table;
    self.tableName      = @(table->name());
    return self;
}

- (NSArray *)selectWithLatitude:(double)latitude lonitude:(double)longitude zoomLevel:(NSUInteger)zoomLevel maxZoomLevel:(NSUInteger)maxZoomLevel
{
    return [CBDataBridge toNSArray:self.nativeInstance->select(latitude, longitude, zoomLevel, maxZoomLevel)];
}

- (NSArray *)selectWithLatitude:(double)latitude lonitude:(double)longitude zoomLevel:(NSUInteger)zoomLevel
{
    return [CBDataBridge toNSArray:self.nativeInstance->select(latitude, longitude, zoomLevel)];
}

- (NSUInteger)insertWithLatitude:(double)latitude longitude:(double)longitude value:(NSDictionary *)value
{
    return self.nativeInstance->insert(latitude, longitude, [CBDataBridge toNativeObject:value]);
}

@end

@implementation CBStorage

- (instancetype)initWithNativeStorage:(cyberbird::Storage *)storage
{
    self = [super init];
    self.nativeInstance = storage;
    return self;
}

- (CBTable *)table:(NSString *)tableName
{
    return [[CBTable alloc] initWithNativeInstance:self.nativeInstance->table([tableName UTF8String])];
}

- (BOOL)createTable:(CBTable *)table
{
    return self.nativeInstance->createTable(table.nativeInstance);
}

- (BOOL)flush
{
    return self.nativeInstance->flush();
}

@end
