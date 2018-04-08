//
//  SSPCoreTests.m
//  SSPCoreTests
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#import <XCTest/XCTest.h>
#include "SLibraryEntrance.hpp"

@interface SSPCoreTests : XCTestCase

@end

@implementation SSPCoreTests

- (void)setUp {
    [super setUp];
    
    SLibraryEntrance::GetInstance()->startSimpleEngine(false);
   
    for (int i = 0; i < 100; i++) {
        SLibraryEntrance::GetInstance()->setHomeScore(i);
        sleep(1);
        SLibraryEntrance::GetInstance()->setMatchTime(1, 0, 0);
    }
    
    [[NSRunLoop currentRunLoop] run];
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
