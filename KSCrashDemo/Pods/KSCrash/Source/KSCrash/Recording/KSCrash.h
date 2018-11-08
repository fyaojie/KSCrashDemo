//
//  KSCrash.h
//
//  Created by Karl Stenerud on 2012-01-28.
//
//  Copyright (c) 2012 Karl Stenerud. All rights reserved.
//
//在此，任何人如欲索取副本，均可获免费许可
//本软件及相关文档文件(以下简称“软件”)的处理
//在软件中不受限制，包括不受限制的权利
//使用、复制、修改、合并、发布、分发、再授权和/或销售
//软件的副本，并允许软件的使用者
//在符合下列条件的情况下，提供下列条件:
//
//上述版权公告及本许可公告应继续有效
//在这个源代码。
//
//本软件是“按原样”提供的，不提供任何形式、明示或其他保证
//包括但不限于适销性保证，
//适合于特定用途和不侵权。在任何情况下都不能
//作者或版权所有者对任何索赔、损害赔偿或其他责任
//责任，无论是在合同诉讼，侵权行为或其他，产生于，
//与软件或使用或其他交易有关的
//该软件。


#import <Foundation/Foundation.h>

#import "KSCrashReportWriter.h"
#import "KSCrashReportFilter.h"
#import "KSCrashMonitorType.h"

typedef enum
{
    KSCrashDemangleLanguageNone = 0,
    KSCrashDemangleLanguageCPlusPlus = 1,
    KSCrashDemangleLanguageSwift = 2,
    KSCrashDemangleLanguageAll = ~1
} KSCrashDemangleLanguage;

typedef enum
{
    KSCDeleteNever,
    KSCDeleteOnSucess,
    KSCDeleteAlways
} KSCDeleteBehavior;

/**
 * 报告应用程序中发生的任何崩溃。
 *
 * 崩溃报告将位于$APP_HOME/Library/ cache /KSCrashReports中
 */
@interface KSCrash : NSObject

#pragma mark - Configuration -

/**  用定制的基路径初始化KSCrash实例*/
- (id) initWithBasePath:(NSString *)basePath;

/**
 *  包含任何你想在崩溃报告中出现的信息的字典。必须
 *  只包含JSON-safe数据:NSString的键，和NSDictionary, NSArray，
 *  NSString, NSDate和NSNumber的值。
 *  默认值: nil
 */
@property(atomic,readwrite,retain) NSDictionary* userInfo;

/** 通过sendAllReportsWithCompletion发送报告后该怎么办:
 *
 *
 * 如果要手动管理报表，请使用KSCDeleteNever。
 * - 如果您要使用警报确认(否则)，请始终使用KSCDeleteAlways 将不断唠叨用户，直到他选择“是”)。
 * 在所有其他情况下使用KSCDeleteOnSuccess。
 *
 * 默认: KSCDeleteAlways
 */
@property(nonatomic,readwrite,assign) KSCDeleteBehavior deleteBehaviorAfterSendAll;

/** 将安装或已经安装的监视器。
 * 注意:如果某些监视器无法安装，那么一旦安装了KSCrash，这个值可能会改变。
 *
 * 默认: KSCrashMonitorTypeProductionSafeMinimal
 */
@property(nonatomic,readwrite,assign) KSCrashMonitorType monitoring;

/** 允许主线程不返回运行的最大时间。
 * 如果任务占用主线程的时间超过此间隔，则watchdog会考虑队列陷入僵局，关闭app并写入一个事故报告。
 *
 * 注意:您必须在监视中添加了KSCrashMonitorTypeMainThreadDeadlock属性，以便产生任何效果
 *
 * 警告:确保你的应用程序中没有任何东西在主线程上运行
 * 完成的时间要比这个值长，否则它会被关闭!这包括
 * 你的应用程序启动过程，所以你可能需要推送应用程序初始化到
 * 另一个线程，或者可能将其设置为更高的值，直到您的应用程序
 * 已完全初始化。

 *
 * 警告:在某些情况下，这仍然会导致误报。自担风险使用!
 *
 * 0 = 禁用.
 *
 * 默认: 0
 */
@property(nonatomic,readwrite,assign) double deadlockWatchdogInterval;

/** 如果是，请在崩溃期间内省内存内容。
 * 栈指针或引用的任何Objective-C对象或C字符串，cpu寄存器或异常将记录在崩溃报告中，他们的内容。
 *
 * 默认: YES
 */
@property(nonatomic,readwrite,assign) BOOL introspectMemory;

/** 如果是，监控所有Objective-C/Swift的交易并跟踪任何交易
 * 回收后访问。
 *
 * 默认: NO
 */
@property(nonatomic,readwrite,assign) BOOL catchZombies;

/**
 * 不应该被反省的Objective-C类的列表。
 * 当遇到这个列表中的类时，只会记录类名。
 * 这对于关注信息安全很有用。
 *
 * Default: nil
 */
@property(nonatomic,readwrite,retain) NSArray* doNotIntrospectClasses;

/** 在删除旧报表之前磁盘上允许的最大报表数量。
 *
 * 默认: 5
 */
@property(nonatomic,readwrite,assign) int maxReportCount;

/** 报告在发送报告的地方下沉。
 * 这必须设置，否则记者将不会发送报告(尽管它会发送报告仍然记录)。
 *
 * 注意:如果使用安装，它会自动设置此属性。
 * 在这种情况下不要修改它。
 */
@property(nonatomic,readwrite,retain) id<KSCrashReportFilter> sink;

/** 在崩溃报告期间调用C函数，以给被调用方一个机会
 * 添加到报告中。NULL =忽略。
 *
 * 警告:只能从这个函数调用异步安全的函数!不叫 objective - c方法! ! !
 *
 * 注意:如果使用安装，它会自动设置此属性。
 * 在这种情况下不要修改它。
 */
@property(nonatomic,readwrite,assign) KSReportWriteCallback onCrash;

/** 将KSCrash控制台日志消息的副本添加到崩溃报告中。
 */
@property(nonatomic,readwrite,assign) BOOL addConsoleLogToReport;

/** 在安装KSCrash时，将上一个应用程序运行日志打印到控制台。
 *  这主要用于调试目的。
 */
@property(nonatomic,readwrite,assign) BOOL printPreviousLog;

/** 在获取堆栈跟踪(默认的KSCrashDemangleLanguageAll)时，应该考虑哪种语言 */
@property(nonatomic,readwrite,assign) KSCrashDemangleLanguage demangleLanguages;

/** 如果从KSCrash设置，将公开uncaughtExceptionHandler。如果调试器正在运行，则为nil。 **/
@property (nonatomic, assign) NSUncaughtExceptionHandler *uncaughtExceptionHandler;

/** 如果从KSCrash设置，则公开currentSnapshotUserReportedExceptionHandler。如果调试器正在运行，则为nil。. **/
@property (nonatomic, assign) NSUncaughtExceptionHandler *currentSnapshotUserReportedExceptionHandler;

#pragma mark - Information - 信息

/** 自上次崩溃以来的总活动时间。 */
@property(nonatomic,readonly,assign) NSTimeInterval activeDurationSinceLastCrash;

/** 自上次崩溃以来的总回溯时间. */
@property(nonatomic,readonly,assign) NSTimeInterval backgroundDurationSinceLastCrash;

/** 自上次崩溃以来应用程序的发布数量. */
@property(nonatomic,readonly,assign) int launchesSinceLastCrash;

/** 自上次崩溃以来的会话数量(启动，从暂停恢复)。 */
@property(nonatomic,readonly,assign) int sessionsSinceLastCrash;

/** 自启动以来的总活动时间。 */
@property(nonatomic,readonly,assign) NSTimeInterval activeDurationSinceLaunch;

/** 自发射以来的总回溯时间。 */
@property(nonatomic,readonly,assign) NSTimeInterval backgroundDurationSinceLaunch;

/** 自应用程序启动以来的会话数量(启动，从暂停恢复)。 */
@property(nonatomic,readonly,assign) int sessionsSinceLaunch;

/** 如果为真，则应用程序在上一次启动时崩溃。 */
@property(nonatomic,readonly,assign) BOOL crashedLastLaunch;

/** 未发送报告的总数。注意:这是一个昂贵的操作。 */
@property(nonatomic,readonly,assign) int reportCount;

/** 关于操作系统和环境的信息 */
@property(nonatomic,readonly,strong) NSDictionary* systemInfo;

#pragma mark - API -

/** 获取崩溃报告器的单例实例。
 */
+ (KSCrash*) sharedInstance;

/**安装事故报告。
 * 记者将记录崩溃，但不会发送任何崩溃报告，除非
 * 水槽。
 *
 * @return 是的，如果记者安装成功。
 */
- (BOOL) install;

/** 将所有未完成的崩溃报告发送到当前的接收器。
 * 只会尝试发送最近的5份报告。所有其他内容将被删除。一旦报告被成功发送到服务器，它们就可能被发送
 * 根据属性“deleteAfterSendAll”在本地删除
 *
 * 注意:必须设置属性“sink”，否则此方法将在完成时调用一个错误。
 *
 * @param onCompletion 发送完成时调用(nil = ignore)。
 */
- (void) sendAllReportsWithCompletion:(KSCrashReportFilterCompletion) onCompletion;

/** 获取所有未发送的报表id。
 *
 * @return 具有报表id的数组。
 */
- (NSArray*) reportIDs;

/** 得到报告。
 *
 * @param reportID 报告的ID。
 *
 * @return 带有报表字段的字典。看到KSCrashReportFields。h表示可用字段。
 */
- (NSDictionary*) reportWithID:(NSNumber*) reportID;

/** 删除所有未发送的报告。
 */
- (void) deleteAllReports;

/** 删除报告。
 *
 * @param reportID 要删除的报告的ID。
 */
- (void) deleteReportWithID:(NSNumber*) reportID;

/** 报告一个自定义的用户定义异常。
 * 这在处理脚本语言时非常有用。
 *
 * 如果终止程序为真，所有岗哨将被卸载，应用程序将以abort()终止。
 *
 * @param name 异常名称(用于命名空间的异常类型)。
 *
 * @param reason 对发生异常的原因的描述。
 *
 * @param language 唯一的语言标识符。
 *
 * @param lineOfCode 有问题的代码行(nil = ignore)的副本。
 *
 * @param stackTrace 表示导致异常(nil = ignore)的调用堆栈的帧(字典或字符串)数组。
 *
 * @param logAllThreads 如果为真，挂起所有线程并记录它们的状态。注意，这将导致性能损失，所以最好只在致命错误上使用。
 *
 * @param terminateProgram 如果为真，请不要从此函数调用返回。终止程序。
 */
- (void) reportUserException:(NSString*) name
                      reason:(NSString*) reason
                    language:(NSString*) language
                  lineOfCode:(NSString*) lineOfCode
                  stackTrace:(NSArray*) stackTrace
               logAllThreads:(BOOL) logAllThreads
            terminateProgram:(BOOL) terminateProgram;

@end


//!KSCrashFramework的项目版本号。
FOUNDATION_EXPORT const double KSCrashFrameworkVersionNumber;

//! KSCrashFramework的项目版本字符串。
FOUNDATION_EXPORT const unsigned char KSCrashFrameworkVersionString[];
