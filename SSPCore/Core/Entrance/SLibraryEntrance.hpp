//
//  SLibraryEntrance.hpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#ifndef SLibraryEntrance_hpp
#define SLibraryEntrance_hpp

//大时间端功能码
#define kFCodeMinuteAdd        0x01//分钟数增加
#define kFCodeMinuteReduce     0x02//分钟数减少
#define kFCodeSecondAdd        0x03//秒数增加
#define kFCodeSecondReduce     0x04//秒数减少
#define kFCode10SecondAdd      0x05//十分之一秒增加
#define kFCode10SecondReduce   0x06//十分之一秒减少
#define kFCodeBigTimeStart     0x07//大时间启动
#define kFCodeBigTimeStop      0x08//大时间停表
#define kFCodeHomeTeamCtrl     0x09//主队球权
#define kFCodeGuestTeamCtrl    0x0a//客队球权
//进攻时间控制功能码
#define kFCodeAttackTimeAdd    0x0b//进攻时间加
#define kFCodeAttackTimeReduce 0x0c//进攻时间减
#define kFCodeAttackTimeStart  0x0d//进攻时间启动
#define kFCodeAttackTimeStop   0x0e//进攻时间暂停
#define kFCodeAttackTimeReset  0x0f//进攻时间复位
#define kFCode14SecondReset    0x10//14秒复位
#define kFCodeAttackTimeZero   0x11//进攻时间清零
//网络码
#define kFCodeConnectSuccess   0x40//连接成功
#define kFCodeConnectFailed    0x41//连接失败
#define kFCodeBeingKickOff     0x42//被其他人挤掉线了
#define kFCodeCtrlSwitchClose  0x43//遥控器控制开关关闭
#define kFCodeNetAbnormal      0x44//网络异常

//多端需要同步的数据
typedef struct SynData {
    unsigned char minute;
    unsigned char second;
    unsigned char mic;
    unsigned char attackSecond;
    unsigned char gameCount;
} StuSynData;

//网络状态和控制端请求的命令码
typedef void (*CmdCallBack)(unsigned char cmd);
//多端数据同步,大时间、进攻时间、当前节数
typedef void (*SysDataCallBack)(StuSynData data);

class SLibraryEntrance
{
public:
    static SLibraryEntrance * GetInstance();
private:
    SLibraryEntrance();
    ~SLibraryEntrance();
public:
    /**
     *  进程推到后台时调用--暂不使用
     */
    void appProjectPending(void);
    /**
     *  进程提到前台时调用--暂不使用
     */
    void appProjectWorking(void);
    /**
     *  释放Core所有资源
     */
    void releaseCoreResources(void);
private:
    /**
     *  释放全局句柄
     */
    void releaseHandle(void);
public:
    /**
     *  初始化启动简版技统
     *
     *  @param  flag Yes表示使用新协议，No表示使用旧协议
     *
     *  @return 失败返回false
     */
    bool startSimpleEngine(bool flag);
    /**
     *  初始化启动专业版技统
     *
     *  @param callBack 底层回调
     *  @param flag Yes表示使用新协议，No表示使用旧协议
     *
     *  @return 失败返回false
     */
    bool startProfessionalEngine(CmdCallBack callBack, bool flag);
    /**
     *  初始化启动遥控器大时间端
     *
     *  @param serverIP     服务器IP地址
     *  @param callBack     底层回调
     *  @param sysCallBack  时间同步回调
     *
     *  @return 失败返回false
     */
    bool startTimerRemoteEngine(const char *serverIP, CmdCallBack callBack, SysDataCallBack sysCallBack);
    /**
     *  初始化启动遥控器进攻时间端
     *
     *  @param serverIP     服务器IP地址
     *  @param callBack     底层回调
     *  @param sysCallBack  时间同步回调
     *
     *  @return 失败返回false
     */
    bool startAttackRemoteEngine(const char *serverIP, CmdCallBack callBack, SysDataCallBack sysCallBack);
public:
    
    /**
     *  设置远程遥控器控制开关
     *
     *  @param isOpen Yes表示允许远程遥控器控制
     */
    void setRemoteCtrlSwitch(bool isOpen);
    /**
     *  设置主队球权
     */
    void setBallCtrlHome(void);
    /**
     *  设置客队球权
     */
    void setBallCtrlGuest(void);
    /**
     *  打开蜂鸣器
     */
    void setOpenDing(void);
    /**
     *  关闭蜂鸣器
     */
    void setCloseDing(void);
    /**
     *  设置当前比赛局数
     *
     *  @param data 当前比赛局数
     */
    void setGameCount(unsigned char data);
    /**
     *  设置当前主队分数
     *
     *  @param data 当前主队分数
     */
    void setHomeScore(unsigned char data);
    /**
     *  设置当前客队分数
     *
     *  @param data 当前客队分数
     */
    void setGuestScore(unsigned char data);
    /**
     *  设置当前主队局分
     *
     *  @param data 当前主队局分
     */
    void setHomeGameScore(unsigned char data);
    /**
     *  设置当前客队局分
     *
     *  @param data 当前客队局分
     */
    void setGuestGameScore(unsigned char data);
    /**
     *  设置当前主队犯规数
     *
     *  @param data 当前主队犯规数
     */
    void setHomeFoulCount(unsigned char data);
    /**
     *  设置当前客队犯规数
     *
     *  @param data 当前客队犯规数
     */
    void setGuestFoulCount(unsigned char data);
    /**
     *  设置当前主队暂停数
     *
     *  @param data 当前主队暂停数
     */
    void setHomePauseCount(unsigned char data);
    /**
     *  设置当前客队暂停数
     *
     *  @param data 当前客队暂停数
     */
    void setGuestPauseCount(unsigned char data);
    /**
     *  设置进攻时间
     *
     *  @param data 进攻时间
     */
    void setAttackSecond(unsigned char data);
    /**
     *  设置比赛时间
     *
     *  @param minute   比赛时间->分
     *  @param second   比赛时间->秒
     *  @param second10 比赛时间->1/10 秒
     */
    void setMatchTime(unsigned char minute, unsigned char second, unsigned char second10);
    /**
     *  大时间启动命令
     */
    void timerStart(void);
    /**
     *  大时间暂停命令
     */
    void timerStop(void);
    /**
     *  大时间分钟加命令
     */
    void timerMinuteAdd(void);
    /**
     *  大时间分钟减命令
     */
    void timerMinuteReduce(void);
    /**
     *  大时间秒加命令
     */
    void timerSecondAdd(void);
    /**
     *  大时间秒减命令
     */
    void timerSecondReduce(void);
    /**
     *  1/10秒加命令
     */
    void timerMilliSecondAdd(void);
    /**
     *  1/10秒减命令
     */
    void timerMilliSecondReduce(void);
    /**
     *  进攻时间启动命令
     */
    void attackStart(void);
    /**
     *  进攻时间暂停命令
     */
    void attackStop(void);
    /**
     *  进攻时间加命令
     */
    void attackSecondAdd(void);
    /**
     *  进攻时间减命令
     */
    void attackSecondReduce(void);
    /**
     *  进攻时间复位
     */
    void attackReset(void);
    /**
     *  进攻时间14秒复位
     */
    void attack14SecondReset(void);
    /**
     *  进攻时间清零
     */
    void attackZero(void);
    /**
     *  关闭机器上的大时间显示
     */
    void closeBigTimeLed(void);
    /**
     *  关闭机器上的进攻时间显示
     */
    void closeAttackTimeLed(void);
};

#endif /* SLibraryEntrance_hpp */
