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

@interface ViewController ()

@property(nonatomic) CyberBirdBridge *cyberBird;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    NSString *dbPath = [NSString stringWithFormat:@"%@/cyberbird.db", [[NSBundle mainBundle] resourcePath]];
    self.cyberBird   = [CyberBirdBridge new];
    [self.cyberBird wake:[NSURL fileURLWithPath:dbPath]];
    NSString *stationFilePath = [[NSBundle mainBundle] pathForResource:@"station" ofType:@"json"];
    NSURL *stationURL         = [NSURL fileURLWithPath:stationFilePath];
    NSString *jsonText        = [[NSString alloc] initWithContentsOfURL:stationURL encoding:NSUTF8StringEncoding error:nil];
    NSData *jsonData          = [jsonText dataUsingEncoding:NSUTF8StringEncoding];
    NSArray *stations         = [NSJSONSerialization JSONObjectWithData:jsonData options:NSJSONReadingMutableContainers error:nil];
    CBStorage *storage        = [self.cyberBird storage];
    CBTableBuilder *builder   = [[CBTableBuilder alloc] initWithTableName:@"stations"];
    CBTable *stationTable     = [[[builder addStringColumn:@"name" size:64] addStringColumn:@"furigana" size:256] build];
    if ([storage createTable:stationTable]) {
        for (NSDictionary *stationData in stations) {
            [stationTable insertWithLatitude:[stationData[@"lat"] floatValue] longitude:[stationData[@"lon"] floatValue]
                                       value:@{ @"name": stationData[@"name"], @"furigana": stationData[@"furigana"] ?: @"" }];
        }
    }
    [storage flush];
    NSArray *foundStations = [self.cyberBird fly:@"stations" latitude:35.65796 longitude:139.708928 zoomLevel:13];
     
    GMSCameraPosition *camera = [GMSCameraPosition cameraWithLatitude:35.65796
                                                            longitude:139.708928
                                                                 zoom:15];
    GMSMapView *mapView = [GMSMapView mapWithFrame:CGRectZero camera:camera];
    self.view           = mapView;
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
