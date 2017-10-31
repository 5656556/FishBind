//
//  IIFishBind.h
//  FishBindDemo
//
//  Created by WELCommand on 2017/9/4.
//  Copyright © 2017年 WELCommand. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface IIFishCallBack : NSObject
@property (nonatomic, weak) id tager;
@property (nonatomic, copy) NSString *selector;
@property (nonatomic, strong) NSArray *args;
@property (nonatomic, strong) id resule;
@end

typedef void(^IIFishCallBackBlock) (IIFishCallBack *callBack, id deadFish);

@interface IIFish : NSObject
@property (nonatomic, assign) NSInteger flag;
@property (nonatomic, weak) id object;
@property (nonatomic, copy) NSString *property;
@property (nonatomic, assign) SEL selector;
@property (nonatomic, copy) IIFishCallBackBlock callBack;

// property bind
+ (instancetype)post:(id)object property:(NSString *)property;
+ (instancetype)observer:(id)object property:(NSString *)property;

// method bind
+ (instancetype)post:(id)object selector:(SEL)selector;
+ (instancetype)observer:(id)object callBack:(IIFishCallBackBlock)callBack;

// bind a block,  using observer:callBack: to observe
+ (instancetype)postBlock:(id)blockObject;

// bilateral bind
+ (instancetype)both:(id)object selector:(SEL)selector callBack:(IIFishCallBackBlock)callBack;
+ (instancetype)both:(id)object property:(NSString *)property callBack:(IIFishCallBackBlock)callBack;

@end

@interface IIFishBind : NSObject
+ (void)bindFishes:(NSArray <IIFish*> *)fishes;
@end

@interface NSObject (IIFishBind)

- (NSArray *)iifish_allKeys;
- (NSArray *)iifish_observersWithKey:(NSString *)key;

- (void)iifish_removeObserverFish:(IIFish *)fish;
- (void)iifish_removeObserverObject:(id)object;
- (void)iifish_removeAllObserver;

@end
