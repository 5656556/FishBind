# FishBind

FishBind可以轻松的实现对象间的绑定。支持绑定属性、方法、block。支持单向绑定&双向绑定。

用这个做MVVM应该很愉快。

项目处于早期版本，仍在持续开发，如果喜欢这个lun zi，赶快一起来加功能、杀bug  ー( ´ ▽ ` )ﾉ

## 例子

```
- (void)viewDidLoad {
    [super viewDidLoad];
    
    //双向绑定

    TestA *objA = [TestA new];
    TestB *objB = [TestB new];
    TestC *objC = [TestC new];
    TestD *objD = [TestD new];
    
    [IIFishBind bindFishes:@[
                             [IIFish both:objA property:@"name"
                                 callBack:^(IIFishCallBack *callBack, id deadFish) {
                                     [deadFish setUserName:callBack.args[0]];
                                 }],
                             [IIFish both:objB property:@"bName"
                                 callBack:^(IIFishCallBack *callBack, id deadFish) {
                                     [deadFish setBName:callBack.args[0]];
                                 }],
                             [IIFish both:objD
                                 selector:@selector(setDK_Name:)
                                 callBack:^(IIFishCallBack *callBack, id deadFish) {
                                     [deadFish setDK_Name:[NSString stringWithFormat:@"DK_%@",callBack.args[0]]];
                                 }],
                             [IIFish observer:objC
                                     callBack:^(IIFishCallBack *callBack, id deadFish) {
                                         objC.fullName = [NSString stringWithFormat:@"\nTestA : name = %@\nTestB : bName = %@\nTestD : DK_Name = %@",objA.userName, objB.bName, objD.DK_Name];
                                     }]
                             ]];
    
    
    objA.name = @"json";
    NSLog(@"%@", objC.fullName);
    /*
     TestA : name = json
     TestB : bName = json
     TestD : DK_Name = DK_json
     */
    
    objB.bName = @"GCD";
    NSLog(@"%@", objC.fullName);
    /*
    TestA : name = GCD
    TestB : bName = GCD
    TestD : DK_Name = DK_GCD
     */
    
    objD.DK_Name = @"apple";
    NSLog(@"%@", objC.fullName);
    /*
    TestA : name = apple
    TestB : bName = apple
    TestD : DK_Name = apple
     */
    
    // 绑定block
    
    CGFloat (^testBlock)(CGFloat i, CGFloat j) = ^(CGFloat i, CGFloat j) {
        return i + j;
    };
    
    [IIFishBind bindFishes:@[
                             [IIFish postBlock:testBlock],
                             [IIFish observer:self
                                     callBack:^(IIFishCallBack *callBack, id deadFish) {
                                         NSLog(@"%@ + %@ = %@", callBack.args[0], callBack.args[1], callBack.resule);
                                         // 3.1 + 4.1 = 7.199999999999999
                                     }]
                             ]];
    
    
    CGFloat value = testBlock (3.1, 4.1);
    
    NSLog(@"value = %@", @(value));
    // value = 7.199999999999999
    
    // 单向绑定
    
    [IIFishBind bindFishes:@[
                             [IIFish post:self selector:@selector(viewDidAppear:)],
                             [IIFish observer:self
                                     callBack:^(IIFishCallBack *callBack, id deadFish) {
                                          NSLog(@"======== 2 ===========");
                                     }]
                             ]];
}


- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    NSLog(@"======== 1 ===========");
}

```

 

## 安装

人肉 IIFishBind.h&.m 到项目 （再做几个功能就加pod）


## 使用

现阶段，基本上就是搞几个IIFish，然后丢给IIFishBind。根据IIFish的语意不同，实现不同的逻辑。

下面是几种IIFish的初始化方式

```
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
+ (instancetype)both:(id)object property:(NSString *)property;
```



## 单向绑定

### 绑定属性

比如，有对象A，希望监控其属性a的变化，则使用

```
IIFish *fish1 = [IIFish post:A property:@"a"];
```

然后，如果希望在a变化后，把a的值直接传给对象B的属性b（现阶段，属性a和b的类型必须相同）， 则使用

```
IIFish *fish2 = [IIFish observer:B property:@"b"];
```

如果，只是希望像KVO一样观察事件，然后做一些复杂的操作，则使用

```
IIFish *fish2 = [IIFish observer:B callBack:^(IIFishCallBack *callBack, id deadFish) {
  
}];
```

IIFishCallBack 会把完整的信息交给你。IIFishCallBack结构如下

```
@interface IIFishCallBack : NSObject
@property (nonatomic, weak) id tager; //被观察者
@property (nonatomic, copy) NSString *selector; //被调用的方法
@property (nonatomic, strong) NSArray *args; // 该方法的参数的值
@property (nonatomic, strong) id resule; // 方法的返回值
@end
```

而deadFish是什么呢？在本例中，deadFish可以理解为对象B，即观察者。假设B有一个方法test被C监控，调用[B test]，C会收到回调。调用[deadFish test]，C不会收到回掉（为什么要设计这个，在下文有介绍）。

最后把两个IIFish对象交给IIFishBind，就完成了绑定，代码如下。

```
[IIFishBind bindFishes:[fish1,fish2]];
```



### 绑定方法

绑定属性其实就是绑定方法。本质上没区别。

在使用上，被监控的对象需要使用

```
+ (instancetype)post:(id)object selector:(SEL)selector;
```

观察者也必须使用

```
+ (instancetype)observer:(id)object callBack:(IIFishCallBackBlock)callBack;
```



### 绑定Block

其实就是钩一个block。在block执行后加一些自己的代码。（感觉上可以做block队列，或者block事件发给多个接收者。不过实际上没遇见这种需求，纯粹是为了好玩才写的.....）

流程上和上面一样。使用下面的方法初始化IIFish。

```
+ (instancetype)postBlock:(id)blockObject;
+ (instancetype)observer:(id)object callBack:(IIFishCallBackBlock)callBack;
```

最后绑定一下就好了。

```
[IIFishBind bindFishes:XXXXX];
```



## 双向绑定

单向绑定的话，属性可以用KVO，方法可以用Aspects，基本都是现成的方案。写这个的目的主要是为了实现双向绑定。

比如，有对象A、B、C，分别有属性a、b、c。三个属性内容一样，希望这三个属性有一个被更改，其它两个跟着被更改。则代码如下

```
[IIFishBind bindFishes:@[
	[IIFish both:A property:@"a"],
	[IIFish both:B property:@"b"],
	[IIFish both:C property:@"c"]
]];
```

如果，对象B的属性b，表示a+c，则如下。

```
[IIFishBind bindFishes:@[
		[IIFish both:A selector:@selector(setA:) callBack:^(IIFishCallBack *callBack, id deadFish) {
      deadFish.a = B.b - C.c;
	}]，
		[IIFish both:C selector:@selector(setC:) callBack:^(IIFishCallBack *callBack, id deadFish) {
      deadFish.c = B.b - A.a;
	}]，
	[IIFish both:B selector:@selector(setB:) callBack:^(IIFishCallBack *callBack, id deadFish) {
      deadFish.b = A.a + C.c;
	}]
]];
```

这里，如果调用 B.b则回会造成死循环，所以需要使用deadFish。

## todo

- [ ] 兼容KVO
- [ ] 适配UIKit
- [ ] 属性类型自动适配
