//
//  ViewController.m
//  CyberBirdExample
//
//  Created by masaaki.goshima on 2017/02/24.
//  Copyright © 2017年 goccy. All rights reserved.
//

#import "ViewController.h"
#import "CyberBirdBridge.h"
#import <GoogleMaps/GoogleMaps.h>

@interface ViewController ()<GMSMapViewDelegate>

@property(nonatomic) CyberBirdBridge *cyberBird;
@property(nonatomic) NSMutableDictionary *cachedMarkers;

@end

@implementation ViewController

static const NSUInteger MARKER_THRESHOLD = 1000;

- (void)viewDidLoad {
    [super viewDidLoad];
    NSString *dbPath = [NSString stringWithFormat:@"%@/tmp/cyberbird.db", NSHomeDirectory()];
    [[NSFileManager defaultManager] removeItemAtPath:dbPath error:nil];
    self.cyberBird   = [CyberBirdBridge new];
    [self.cyberBird wake:[NSURL fileURLWithPath:dbPath]];
    self.cachedMarkers = [@{} mutableCopy];
    NSString *stationFilePath = [[NSBundle mainBundle] pathForResource:@"station" ofType:@"json"];
    NSURL *stationURL         = [NSURL fileURLWithPath:stationFilePath];
    NSString *jsonText        = [[NSString alloc] initWithContentsOfURL:stationURL encoding:NSUTF8StringEncoding error:nil];
    NSData *jsonData          = [jsonText dataUsingEncoding:NSUTF8StringEncoding];
    NSArray *stations         = [NSJSONSerialization JSONObjectWithData:jsonData options:NSJSONReadingMutableContainers error:nil];
    CBStorage *storage        = [self.cyberBird storage];
    CBTableBuilder *builder   = [[CBTableBuilder alloc] initWithTableName:@"stations"];
    CBTable *stationTable     = [[[builder addStringColumn:@"name" size:64] addStringColumn:@"furigana" size:256] build];
    if ([storage createTable:stationTable]) {
        NSMutableDictionary *uniqueStationMap = [@{} mutableCopy];
        for (NSDictionary *stationData in stations) {
            uniqueStationMap[stationData[@"name"]] = stationData;
        }
        for (NSDictionary *stationData in [uniqueStationMap allValues]) {
            [stationTable insertWithLatitude:[stationData[@"lat"] doubleValue] longitude:[stationData[@"lng"] doubleValue]
                                       value:@{ @"name": stationData[@"name"], @"furigana": stationData[@"furigana"] ?: @"" }];
        }
    }
    [storage flush];
    
    GMSCameraPosition *camera = [GMSCameraPosition cameraWithLatitude:35.65796
                                                            longitude:139.708928
                                                                 zoom:15];
    GMSMapView *mapView = [GMSMapView mapWithFrame:CGRectZero camera:camera];
    mapView.delegate    = self;
    self.view           = mapView;
}

- (void)mapView:(GMSMapView *)mapView didChangeCameraPosition:(GMSCameraPosition *)position
{
    if (position.zoom < 13) return;
    
    if ([[self.cachedMarkers allValues] count] > MARKER_THRESHOLD) {
        @autoreleasepool {
            [mapView clear];
            [self.cachedMarkers removeAllObjects];
        }
    }
    
    GMSProjection *projection = mapView.projection;
    CLLocationCoordinate2D centerLocation = [projection coordinateForPoint:CGPointMake(mapView.frame.size.width / 2.0f, mapView.frame.size.height / 2.0f)];
    [self.cyberBird fly:@"stations"
               latitude:centerLocation.latitude
              longitude:centerLocation.longitude
              zoomLevel:position.zoom
          didCompletion:^(NSArray *foundLocations) {
              for (NSDictionary *station in foundLocations) {
                  NSUInteger key = [station[@"id"] intValue];
                  GMSMarker *cachedMarker = self.cachedMarkers[@(key)];
                  if (cachedMarker) {
                      if (!cachedMarker.map) cachedMarker.map = mapView;
                  } else {
                      CLLocationCoordinate2D location = CLLocationCoordinate2DMake([station[@"latitude"] doubleValue], [station[@"longitude"] doubleValue]);
                      GMSMarker *marker       = [GMSMarker markerWithPosition:location];
                      marker.title            = station[@"name"];
                      marker.appearAnimation  = kGMSMarkerAnimationPop;
                      marker.map              = mapView;
                      self.cachedMarkers[@(key)] = marker;
                  }
              }
          }];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
