//
//  SProfessionalManager.hpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

//专业版
#ifndef SProfessionalManager_hpp
#define SProfessionalManager_hpp

#include "../Public/STcpServer.hpp"
#include "SSimpleManager.hpp"

class SProfessionalManager : public SSimpleManager
{
public:
    SProfessionalManager();
    ~SProfessionalManager();
public:
    /**
     *  初始化专业版计统
     *
     *  @param bFlag true表示允许
     *  @return 失败返回false
     */
    bool initProfessional(CmdCallBack callBack);
    /**
     *  是否允许远程遥控器控制
     *
     *  @param bFlag true表示允许
     */
    void remoteCtrlSwitch(bool bFlag);
private:
    /**
     *  专业版主要业务逻辑线程
     *
     *  @param obj 传入线程参数
     */
    static void *logicThreadFuc(void *obj);
    /**
     *  与进攻时间端通信业务逻辑线程
     *
     *  @param obj 传入线程参数
     */
    static void *attackCommThreadFuc(void *obj);
    /**
     *  与大时间端通信业务逻辑线程
     *
     *  @param obj 传入线程参数
     */
    static void *timerCommThreadFuc(void *obj);
    /**
     *  客户端连进来后，读取数据，进行识别处理
     *
     *  @param clientInfo    在线的客户端信息
     *  @param newClientInfo 新连接进来的客户端信息
     */
    void clientConnCheckDealwith(StuClientInfo *clientInfo, StuClientInfo newClientInfo);
    /**
     *  解析数据包
     *
     *  @param data 数据包
     */
    void dataPacketAnalysis(char *data);
    /**
     *  关闭进攻时间客户端TCP Socket
     */
    void closeAttackTcpSocket(void);
    /**
     *  关闭大时间客户端TCP Socket
     */
    void closeTimerTcpSocket(void);
    /**
     *  关闭TCP相关Socket
     */
    void closeTcpSocket(void);
private:
    bool            m_bRemoteCtrlSwitch;//是否允许遥控器控制
    bool            m_threadSwitch;//线程开关
    pthread_t       m_logicThread;//主逻辑业务线程
    pthread_t       m_attackCommThread;//与进攻时间端通信线程
    pthread_t       m_timerCommThread;//与大时间端通信线程
    
    STcpServer      *m_tcpServer;
    CmdCallBack     m_cmdCallBack;
    StuClientInfo   m_attackClientInfo;
    StuClientInfo   m_timerClientInfo;
};

#endif /* SProfessionalManager_hpp */
