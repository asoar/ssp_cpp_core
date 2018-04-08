//
//  SRemoteManager.cpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#include "SRemoteManager.hpp"

SRemoteManager::SRemoteManager()
{
    pthread_mutex_init(&m_mutex, NULL);
    setThreadSwitch(true);
    m_remoteType = BigTimeRemoteType;
    
    m_udpClient = new SUdpClient();
    m_tcpClient = new STcpClient();
    m_tcpLogicThread = NULL;
    m_udpRecvThread = NULL;
}

SRemoteManager::~SRemoteManager()
{
    setThreadSwitch(false);
    pthread_mutex_destroy(&m_mutex);//释放锁资源
    
    //子线程的状态设置为分离,使join变成非阻塞==暂不用
    //pthread_detach(m_tcpLogicThread);
    if(pthread_kill(m_tcpLogicThread, 0) != ESRCH){//判断线程是否存活
        pthread_join(m_tcpLogicThread,NULL);//回收线程资源==阻塞
    }
    if (pthread_kill(m_udpRecvThread, 0) != ESRCH) {
        pthread_join(m_udpRecvThread,NULL);
    }
    
    delete m_udpClient;
    delete m_tcpClient;
    
    m_udpClient = NULL;
    m_tcpClient = NULL;
    m_tcpLogicThread = NULL;
    m_udpRecvThread = NULL;
}

#pragma mark 初始化大时间遥控器
bool SRemoteManager::initBigTimeRemote(const char *serverIP, CmdCallBack callBack, SysDataCallBack sysCallBack)
{
    m_remoteType = BigTimeRemoteType;
    return initPublicRemote(serverIP, callBack, sysCallBack);
}

#pragma mark 初始化进攻时间遥控器
bool SRemoteManager::initAttackRemote(const char *serverIP, CmdCallBack callBack, SysDataCallBack sysCallBack)
{
    m_remoteType = AttackRemoteType;
    return initPublicRemote(serverIP, callBack, sysCallBack);
}

bool SRemoteManager::initPublicRemote(const char *serverIP, CmdCallBack callBack, SysDataCallBack sysCallBack)
{
    m_callBack = callBack;
    m_sysCallBack = sysCallBack;
    bzero(m_serverIP, 16);
    strncpy(m_serverIP, serverIP, strlen(serverIP)+1);

    if (!m_tcpLogicThread && (pthread_create(&m_tcpLogicThread, NULL, logicThreadFuc, this) != 0)) return false;
    if (!m_udpRecvThread && (pthread_create(&m_udpRecvThread, NULL, udpRecvThreadFuc, this) != 0)) return false;
    
    return true;
}

#pragma mark 初始化UDP Socket 接收广播数据，与技统同步数据
bool SRemoteManager::initUDPSocketRecvData(void)
{
    m_udpClient->setRecvBroadcastAddr();
    if (!m_udpClient->createSock())
        return false;
    if (!m_udpClient->bindSock())
        return false;
    return true;
}

#pragma mark ====向服务端发送命令====
void SRemoteManager::sendCmdToServer(unsigned char cmd)
{
    if (!m_threadSwitch) {
        printf("交互都结束了，还操作个毛线");
        if (m_callBack){
            m_callBack(kFCodeNetAbnormal);
        }
        return;
    }
    
    char *data = dataPackage(cmd, NULL);
    m_tcpClient->sendDataToServer(data, data[3]);
    delete data;
    data = NULL;
}


#pragma mark ====与服务端通信主要业务逻辑线程=====
void *SRemoteManager::logicThreadFuc(void *obj)
{
    SRemoteManager *p = (SRemoteManager*)obj;
    while (p->m_threadSwitch) {
        if (!p->m_tcpClient->initClientAddr(p->m_serverIP, kTCPPort)) {
            printf("===SRemoteManager==tcp==initClientAddr异常\n");
            usleep(10000);//10毫秒
            continue;
        }
        if (!p->m_tcpClient->createSock()) {
            printf("===SRemoteManager==tcp==createSock异常\n");
            p->m_callBack(kFCodeNetAbnormal);
            usleep(10000);//10毫秒
            continue;
        }
        if (!p->m_tcpClient->connServer()) {
            printf("===SRemoteManager==tcp==connServer异常\n");
            p->m_callBack(kFCodeConnectFailed);
            usleep(10000);//10毫秒
            continue;
        }
        if(!p->initUDPSocketRecvData()){
            printf("===SRemoteManager==udp==初始化异常\n");
            p->m_callBack(kFCodeNetAbnormal);
            usleep(10000);//10毫秒
            continue;
        }
        //连接成功后，进行身份识别
        if(!p->checkIdentityAuth()){
            printf("身份未能识别！\n");
            continue;
        }
        p->m_callBack(kFCodeConnectSuccess);//连接成功
        
        char buffer[100] = {0,};
        while (p->m_threadSwitch) {
            bzero(buffer, 100);
            if (p->m_tcpClient->recvDataFromServer(buffer, 100) > 0){
                //读到服务端发来的数据,进行处理
                p->analysisDataPacket(buffer);
                p->printHex(buffer, strlen(buffer));
            } else {
                //阻塞读取出错，说明连接出问题了，应不再读取，重新建立连接
                printf("======貌似网路有点异常啊，读取服务器数据出错了!======");
                if (p->m_callBack){
                    p->m_callBack(kFCodeNetAbnormal);
                }
                break;
            }
        }
        printf("==========网络异常了，准备重新连接服务器==========");
    }
    printf("==SRemoteManager==业务线程被释放\n");
    return NULL;
}

#pragma mark ====接收广播数据，进行数据同步=====
void *SRemoteManager::udpRecvThreadFuc(void *obj)
{
    SRemoteManager *p = (SRemoteManager*)obj;
    StuSynData sysData;
    
    while (p->m_threadSwitch) {
        unsigned char buffer[37] = {0,};
        char ipStr[20] = "";
        if(p->m_udpClient->recvData(buffer, 37, ipStr) != -1){
            if(p->checkUdpDataPack(buffer)){//先校验一下接收到的UDP数据包
                sysData.gameCount = buffer[8];
                sysData.attackSecond = buffer[11];
                sysData.minute = buffer[12];
                sysData.second = buffer[13];
                sysData.mic = buffer[14];
                p->m_sysCallBack(sysData);
            }
            continue;
        }
        usleep(10000);
    }
    printf("======接收广播数据线程终止咯========\n");
    return NULL;
}

#pragma mark =====与服务端进行校验身份认证=====
bool SRemoteManager::checkIdentityAuth(void)
{
    unsigned char identity = (m_remoteType == AttackRemoteType ? kFCodeAttackIdentity : kFCodeTimerIdentity);
    char *data = dataPackage(identity, NULL);
    long n = m_tcpClient->sendDataToServer(data, data[3]);
    delete data;
    data = NULL;
    if (n < 0) return false;
    
    char buffer[100];
    bzero(buffer, 100);
    long m = m_tcpClient->recvDataFromServer(buffer, 100);
    if (m > 0 && buffer[4] == kFCodeConnectSuccess) {
        return true;
    }
    
    return false;
}

#pragma mark =====解析服务端发来的tcp数据包=====
void SRemoteManager::analysisDataPacket(char *data)
{
    if (data[4] == kFCodeBeingKickOff) {
        setThreadSwitch(false);
    }
    if (m_callBack) {
        m_callBack(data[4]);
    }
}

#pragma mark =====改变线程开关的值===先锁住====
void SRemoteManager::setThreadSwitch(bool flag)
{
    pthread_mutex_lock(&m_mutex);
    m_threadSwitch = flag;
    pthread_mutex_unlock(&m_mutex);
}
