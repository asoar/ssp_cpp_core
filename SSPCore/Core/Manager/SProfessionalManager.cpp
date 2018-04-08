//
//  SProfessionalManager.cpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#include "SProfessionalManager.hpp"

pthread_mutex_t gProfessionalMutex =
PTHREAD_MUTEX_INITIALIZER;

SProfessionalManager::SProfessionalManager()
{
    m_bRemoteCtrlSwitch = false;//默认是不允许遥控器控制的
    m_threadSwitch = true;//默认打开线程开关
    
    m_logicThread = NULL;
    m_attackCommThread = NULL;
    m_timerCommThread = NULL;
    m_cmdCallBack = NULL;
    
    m_tcpServer = new STcpServer();

    m_attackClientInfo.clientSock = -1;
    memset((void *)&this->m_attackClientInfo.clientAddr, 0, sizeof(this->m_attackClientInfo.clientAddr));
    
    m_timerClientInfo.clientSock = -1;
    memset((void *)&this->m_timerClientInfo.clientAddr, 0, sizeof(this->m_timerClientInfo.clientAddr));
}

SProfessionalManager::~SProfessionalManager()
{
    m_threadSwitch = false;//先关闭线程开关，再释放其他资源
    closeTcpSocket();
    
    //子线程的状态设置为分离,使join变成非阻塞==暂不用
    //pthread_detach(m_recvThread);
    if(pthread_kill(m_logicThread, 0) != ESRCH){//判断线程是否存活
        pthread_join(m_logicThread,NULL);//回收线程资源==阻塞
    }
    if(pthread_kill(m_attackCommThread, 0) != ESRCH){
        pthread_join(m_attackCommThread,NULL);
    }
    if(pthread_kill(m_timerCommThread, 0) != ESRCH){
        pthread_join(m_timerCommThread,NULL);
    }
    m_logicThread = NULL;
    m_attackCommThread = NULL;
    m_timerCommThread = NULL;
    
    if (m_tcpServer) {
        delete m_tcpServer;
        m_tcpServer = NULL;
    }
    pthread_mutex_destroy(&gProfessionalMutex);//销毁锁资源
}

bool SProfessionalManager::initProfessional(CmdCallBack callBack)
{
    if (!callBack) return false;
    m_cmdCallBack = callBack;
    
    if (!m_logicThread && (pthread_create(&m_logicThread, NULL, logicThreadFuc, this) != 0)) return false;
    
    if (!m_attackCommThread && (pthread_create(&m_attackCommThread, NULL, attackCommThreadFuc, this) != 0)) return false;
    
    if (!m_timerCommThread && (pthread_create(&m_timerCommThread, NULL, timerCommThreadFuc, this) != 0)) return false;
    
    return true;
}

#pragma mark =====设置是否允许遥控器控制======
void SProfessionalManager::remoteCtrlSwitch(bool bFlag)
{
    m_bRemoteCtrlSwitch = bFlag;
}

#pragma mark =====专业版主要业务逻辑线程======
void *SProfessionalManager::logicThreadFuc(void *obj)
{
    SProfessionalManager *pObj = (SProfessionalManager *)obj;
    StuClientInfo newClientInfo;
    char readBuffer[100]={0,};

    while (pObj->m_threadSwitch) {
        
        pObj->m_tcpServer->initServerAddr(kTCPPort);
        if (!pObj->m_tcpServer->createSock()) {
            usleep(10000);//10毫秒
            printf("=======tcp server socket init failed=====\n");
            continue;
        }
        if(!pObj->initSimple()){
            usleep(10000);//10毫秒
            printf("=======udp socket init failed=====\n");
            continue;
        }
        if (!pObj->m_tcpServer->bindSock()) {
            usleep(10000);//10毫秒
            printf("=======tcp server bind sock failed=====\n");
            continue;
        }
        if (!pObj->m_tcpServer->listenSocket()) {
            usleep(10000);//10毫秒
            printf("=======tcp server listen sock failed=====\n");
            continue;
        }
        
        while(pObj->m_threadSwitch) {
            bzero(readBuffer, 100);
            newClientInfo.clientSock = -1;
            memset((void *)&newClientInfo.clientAddr, 0, sizeof(newClientInfo.clientAddr));
            
            if(!pObj->m_tcpServer->acceptSocket(&newClientInfo)){
                usleep(10000);
                break;
            }
            
            //连接后,读取客户端身份信息进行校验(大时间、24秒)
            if(pObj->m_tcpServer->readDataFormClient(&newClientInfo, readBuffer, 100) < 0){
                close(newClientInfo.clientSock);
                newClientInfo.clientSock = -1;
                continue;
            }
            if (readBuffer[4] == kFCodeAttackIdentity) {
                pObj->clientConnCheckDealwith(&pObj->m_attackClientInfo, newClientInfo);
            } else if (readBuffer[4] == kFCodeTimerIdentity) {
                pObj->clientConnCheckDealwith(&pObj->m_timerClientInfo, newClientInfo);
            }
        }
    }
    printf("========server逻辑线程被主动挂掉=========\n");
    
    return NULL;
}

#pragma mark =====客户端连进来后，读取数据，进行识别处理======
void SProfessionalManager::clientConnCheckDealwith(StuClientInfo *clientInfo, StuClientInfo newClientInfo)
{
    char *pConnSuccessData = NULL;
    char *pData = NULL;
    
    //告诉在线的客户端，你被其他人挤掉了
    if (clientInfo->clientSock != -1) {
        pData = dataPackage(kFCodeBeingKickOff, NULL);
        m_tcpServer->sendDataToClient(clientInfo, pData, pData[3]);
        close(clientInfo->clientSock);
        clientInfo->clientSock = -1;
        delete pData;
        pData = NULL;
    }
    //告诉新连接的客户端，身份已识别==即连接成功
    pConnSuccessData = dataPackage(kFCodeConnectSuccess, NULL);
    clientInfo->clientSock = newClientInfo.clientSock;
    clientInfo->clientAddr = newClientInfo.clientAddr;
    m_tcpServer->sendDataToClient(clientInfo, pConnSuccessData, pConnSuccessData[3]);
    delete pConnSuccessData;
    pConnSuccessData = NULL;
}


#pragma mark =====与进攻时间端通信业务逻辑线程=====
void *SProfessionalManager::attackCommThreadFuc(void *obj)
{
    SProfessionalManager *pObj = (SProfessionalManager *)obj;
    
    char buffer[100] = {0,};
    while (pObj->m_threadSwitch) {
        if (pObj->m_attackClientInfo.clientSock == -1) {
            usleep(10000);
            continue;
        }
        
        bzero(buffer, 100);
        if (pObj->m_tcpServer->readDataFormClient(&pObj->m_attackClientInfo, buffer, 100) <= 0) {
            ////客户端连接异常了===不需要处理异常(等待新的连接)
            usleep(10000);
            printf("遥控器24s客户端连接异常!==>%s\n", inet_ntoa(pObj->m_attackClientInfo.clientAddr.sin_addr));
            continue;
        }
        pObj->printHex(buffer, buffer[4]);
        
        unsigned char fcode = (unsigned char)buffer[4];
        //64以内表示客户端命令码,不允许控制的情况下就不处理,并告诉客户端
        if (fcode < 64 && !pObj->m_bRemoteCtrlSwitch) {
            char *pData = NULL;
            pData = pObj->dataPackage(kFCodeCtrlSwitchClose, NULL);
            pObj->m_tcpServer->sendDataToClient(&pObj->m_attackClientInfo, pData, pData[3]);
            delete pData;
            pData = NULL;
        } else {
            pthread_mutex_lock(&gProfessionalMutex);
            pObj->dataPacketAnalysis(buffer);
            pthread_mutex_unlock(&gProfessionalMutex);
        }
    }
    printf("客户端:遥控器24s线程挂掉\n");
    
    return NULL;
}

#pragma mark =====与大时间端通信业务逻辑线程=====
void *SProfessionalManager::timerCommThreadFuc(void *obj)
{
    SProfessionalManager *pObj = (SProfessionalManager *)obj;
    
    char buffer[100] = {0,};
    while (pObj->m_threadSwitch) {
        if (pObj->m_timerClientInfo.clientSock == -1) {
            usleep(10000);
            continue;
        }
        
        bzero(buffer, 100);
        if (pObj->m_tcpServer->readDataFormClient(&pObj->m_timerClientInfo, buffer, 100) <= 0) {
            ////客户端连接异常了===不需要处理异常(等待新的连接)
            usleep(10000);
            printf("遥控器Timer客户端连接异常!==>%s\n", inet_ntoa(pObj->m_timerClientInfo.clientAddr.sin_addr));
            continue;
        }
        pObj->printHex(buffer, buffer[3]);
        
        unsigned char fcode = (unsigned char)buffer[4];
        //64以内表示客户端命令码,不允许控制的情况下就不处理并告诉客户端
        if (fcode < 64 && !pObj->m_bRemoteCtrlSwitch) {
            char *pData = NULL;
            pData = pObj->dataPackage(kFCodeCtrlSwitchClose, NULL);
            pObj->m_tcpServer->sendDataToClient(&pObj->m_timerClientInfo, pData, pData[3]);
            delete pData;
            pData = NULL;
        } else {
            pthread_mutex_lock(&gProfessionalMutex);
            pObj->dataPacketAnalysis(buffer);
            pthread_mutex_unlock(&gProfessionalMutex);
        }
    }
    printf("客户端:遥控器Timer线程挂掉\n");
    
    return NULL;
}

#pragma mark 解析数据包===如果豪华版没有准备好，是不允许遥控器控制的
void SProfessionalManager::dataPacketAnalysis(char *data)
{
    m_cmdCallBack(data[4]);//因协议过于简单不需要解包
}

#pragma mark =====关闭进攻时间客户端TCP Socket=====
void SProfessionalManager::closeAttackTcpSocket(void)
{
    if (m_attackClientInfo.clientSock != -1) {
        close(m_attackClientInfo.clientSock);
        m_attackClientInfo.clientSock = -1;
    }
}

#pragma mark =====关闭大时间客户端TCP Socket=====
void SProfessionalManager::closeTimerTcpSocket(void)
{
    if (m_timerClientInfo.clientSock != -1) {
        close(m_timerClientInfo.clientSock);
        m_timerClientInfo.clientSock = -1;
    }
}

#pragma mark =====关闭TCP相关Socket=====
void SProfessionalManager::closeTcpSocket(void)
{
    closeAttackTcpSocket();
    closeTimerTcpSocket();
    m_tcpServer->closeServerSock();
}
