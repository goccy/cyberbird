//
//  CBStorage.h
//  Pods
//
//  Created by masaaki.goshima on 2017/02/23.
//
//

#import <Foundation/Foundation.h>

@class CBTable;

@interface CBTableBuilder : NSObject

@property(nonatomic, readonly) NSString *tableName;

- (instancetype)initWithTableName:(NSString *)tableName;
- (instancetype)addNumberColumn:(NSString *)columnName;
- (instancetype)addStringColumn:(NSString *)columnName size:(NSUInteger)columnSize;
- (CBTable *)build;

@end

@interface CBTable : NSObject

@property(nonatomic, readonly) NSString *tableName;

- (NSArray *)selectWithLatitude:(double)latitude lonitude:(double)longitude zoomLevel:(NSUInteger)zoomLevel maxZoomLevel:(NSUInteger)maxZoomLevel;
- (NSArray *)selectWithLatitude:(double)latitude lonitude:(double)longitude zoomLevel:(NSUInteger)zoomLevel;
- (NSUInteger)insertWithLatitude:(double)latitude longitude:(double)longitude value:(NSDictionary *)value;

@end

@interface CBStorage : NSObject

- (CBTable *)table:(NSString *)tableName;
- (BOOL)createTable:(CBTable *)table;
- (BOOL)flush;

@end
