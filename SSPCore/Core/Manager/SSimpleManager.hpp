//
//  SSimpleManager.hpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//
//简版

#ifndef SSimpleManager_hpp
#define SSimpleManager_hpp

#include "../Public/SUdpClient.hpp"

class SSimpleManager : public SObject
{
public:
    SSimpleManager();
    ~SSimpleManager();
public:
    /**
     *  是否切换为新协议
     *
     *  @param flag true表示切换为新协议，false表示切换为旧协议
     */
    void isCheckNewestProtocol(bool flag);
    /**
     *  初始化简版计统
     *
     *  @return 失败返回false
     */
    bool initSimple(void);
    /**
     *  更新主队当前分数
     *
     *  @param score 当前主队分数
     */
    void setHomeScore(unsigned char score);
    /**
     *  更新客队当前分数
     *
     *  @param score 当前客队分数
     */
    void setGuestScore(unsigned char score);
    /**
     *  更新当前节数
     *
     *  @param count 当前节数
     */
    void setGameCount(unsigned char count);
    /**
     *  更新主队当节分数
     *
     *  @param score 当前主队当节分数
     */
    void setHomeGameScore(unsigned char score);
    /**
     *  更新客队当节分数
     *
     *  @param score 当前客队当节分数
     */
    void setGuestGameScore(unsigned char score);
    /**
     *  更新主队当前总犯规数
     *
     *  @param count 当前主队总犯规数
     */
    void setHomeFoulCount(unsigned char count);
    /**
     *  更新客队当前总犯规数
     *
     *  @param count 当前客队总犯规数
     */
    void setGuestFoulCount(unsigned char count);
    /**
     *  更新主队当前总暂停数
     *
     *  @param count 当前主队总暂停数
     */
    void setHomePauseCount(unsigned char count);
    /**
     *  更新客队当前总暂停数
     *
     *  @param count 当前客队总暂停数
     */
    void setGuestPauseCount(unsigned char count);
    /**
     *  更新当前进攻时间
     *
     *  @param second 当前进攻时间
     */
    void setAttackSecond(unsigned char second);
    /**
     *  更新当节总时间分钟数
     *
     *  @param minute 当前当节总时间分钟数
     */
    void setMinute(unsigned char minute);
    /**
     *  更新当节总时间秒钟数
     *
     *  @param second 当前当节总时间秒钟数
     */
    void setSecond(unsigned char second);
    /**
     *  更新当节总时间1/10秒钟数
     *
     *  @param second10 当前当节总时间1/10秒钟数
     */
    void set1_10Second(unsigned char second10);
    /**
     *  更新当前球队控制权
     *
     *  @param isHomeTeam 当前球队控制权是否是主队
     */
    void setBallControl(bool isHomeTeam);
    /**
     *  更新当前蜂鸣器起停
     *
     *  @param isDing 当前蜂鸣器是否响起
     */
    void setDing(bool isDing);
    /**
     *  广播发送数据
     */
    void sendData(void);
    /**
     *  设置关闭机器大时间显示
     */
    void setCloseBigTimeLed(void);
    /**
     *  设置关闭机器进攻时间显示
     */
    void setCloseAttackTimeLed(void);
private:
    SUdpClient *m_pUDPClient;
    unsigned char m_dataProtocol[37];
};


#endif /* SSimpleManager_hpp */
