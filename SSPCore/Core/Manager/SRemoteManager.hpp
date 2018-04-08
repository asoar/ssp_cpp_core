//
//  SRemoteManager.hpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#ifndef SRemoteManager_hpp
#define SRemoteManager_hpp

#include "SUdpClient.hpp"
#include "STcpClient.hpp"

enum RemoteType
{
    //遥控器-计时器
    BigTimeRemoteType = 0,
    //遥控器-24秒
    AttackRemoteType
};

class SRemoteManager : public SObject
{
public:
    SRemoteManager();
    ~SRemoteManager();
public:
    /**
     *  初始化大时间遥控器
     *
     *  @param serverIP     服务器IP地址
     *  @param callBack     底层回调
     *  @param sysCallBack  时间同步回调
     */
    bool initBigTimeRemote(const char *serverIP, CmdCallBack callBack, SysDataCallBack timeCallBack);
    /**
     *  初始化进攻时间遥控器
     *
     *  @param serverIP     服务器IP地址
     *  @param callBack     底层回调
     *  @param sysCallBack  数据同步回调
     */
    bool initAttackRemote(const char *serverIP, CmdCallBack callBack, SysDataCallBack sysCallBack);
    /**
     *  向服务端发送命令
     *
     *  @return 失败返回false
     */
    void sendCmdToServer(unsigned char cmd);
private:
    /**
     *  初始化遥控器公共方法
     *
     *  @param serverIP     服务器IP地址
     *  @param callBack     底层回调
     *  @param sysCallBack  数据同步回调
     */
    bool initPublicRemote(const char *serverIP, CmdCallBack callBack, SysDataCallBack sysCallBack);
    /**
     *  初始化UDP Socket 接收广播数据，与技统同步数据
     *
     *  @return 失败返回false
     */
    bool initUDPSocketRecvData(void);
private:
    /**
     *  与服务端通信主要业务逻辑线程
     *
     *  @param obj 传入线程参数
     */
    static void *logicThreadFuc(void *obj);
    /**
     *  接收广播数据，进行数据同步
     *
     *  @param obj 传入线程参数
     */
    static void *udpRecvThreadFuc(void *obj);
    /**
     *  与服务端进行校验身份认证
     *
     *  @return 校验失败返回false
     */
    bool checkIdentityAuth(void);
    /**
     *  解析服务端发来的tcp数据包
     *
     *  @param data 数据包
     */
    void analysisDataPacket(char *data);
    /**
     *  改变线程开关的值===先锁住
     *
     *  @param flag 写入的值
     */
    void setThreadSwitch(bool flag);
private:
    RemoteType      m_remoteType;
    SUdpClient      *m_udpClient;//用于接收广播数据
    STcpClient      *m_tcpClient;//用于和技统交互数据
    CmdCallBack     m_callBack;
    SysDataCallBack m_sysCallBack;
    
    pthread_mutex_t m_mutex;
    pthread_t       m_tcpLogicThread;
    pthread_t       m_udpRecvThread;
    
    char            m_serverIP[16];//保存服务端IP
    bool            m_threadSwitch;//线程开关，在主线程和业务线程中都有改值，所有要加锁
};

#endif /* SRemoteManager_hpp */
