//
//  SLibraryEntrance.cpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#include "SLibraryEntrance.hpp"
#include "../Manager/SSimpleManager.hpp"
#include "../Manager/SRemoteManager.hpp"
#include "../Manager/SProfessionalManager.hpp"

//应用类型
enum SSPType
{   //未知类型
    UnknownType = 0,
    //遥控器-进攻时间
    RemoteAttackSSPType,
    //遥控器-计时器
    RemoteTimerSSPType,
    //简版技统
    SimpleSSPType,
    //专业版技统
    ProfessionalSSPType,
};

static SSimpleManager       *g_simpleHandle = NULL;
static SRemoteManager       *g_remoteHandle = NULL;
static SProfessionalManager *g_professionalHandle = NULL;
static SLibraryEntrance     *g_libEntrance = NULL;
static SSPType               g_type;

SLibraryEntrance* SLibraryEntrance::GetInstance()
{
    if(!g_libEntrance)
        g_libEntrance = new SLibraryEntrance();
    return g_libEntrance;
}

SLibraryEntrance::SLibraryEntrance()
{
    g_type = UnknownType;
    g_simpleHandle = NULL;
    g_remoteHandle = NULL;
    g_professionalHandle = NULL;
}

SLibraryEntrance::~SLibraryEntrance()
{
    
}

#pragma mark 进程推到后台时,关闭socket释放资源
void SLibraryEntrance::appProjectPending(void)
{
}

#pragma mark 进程提到前台
void SLibraryEntrance::appProjectWorking(void)
{
}

#pragma mark 不再需要时，释放Core所有资源
void SLibraryEntrance::releaseCoreResources(void)
{
    g_type = UnknownType;
    this->releaseHandle();
}

#pragma mark  释放全局句柄
void SLibraryEntrance::releaseHandle(void)
{
    if (g_simpleHandle) {
        delete g_simpleHandle;
        g_simpleHandle = NULL;
    }
    if (g_remoteHandle) {
        delete g_remoteHandle;
        g_remoteHandle = NULL;
    }
    if (g_professionalHandle) {
        delete g_professionalHandle;
        g_professionalHandle = NULL;
    }
}

#pragma mark 初始化启动简版技统
bool SLibraryEntrance::startSimpleEngine(bool flag)
{
    this->releaseHandle();
    g_type = SimpleSSPType;
    
    if (!g_simpleHandle) g_simpleHandle = new SSimpleManager();
    g_simpleHandle->isCheckNewestProtocol(flag);
    
    return g_simpleHandle->initSimple();
}

#pragma mark 初始化启动专业版技统
bool SLibraryEntrance::startProfessionalEngine(CmdCallBack callBack, bool flag)
{
    if (!callBack){
        printf("回调函数不能为nil\n");
        return false;
    }
    
    this->releaseHandle();
    g_type = ProfessionalSSPType;
    
    if (!g_professionalHandle) g_professionalHandle = new SProfessionalManager();
    g_professionalHandle->isCheckNewestProtocol(flag);
    
    return g_professionalHandle->initProfessional(callBack);
}

#pragma mark ======初始化启动遥控器大时间端======
bool SLibraryEntrance::startTimerRemoteEngine(const char *serverIP, CmdCallBack callBack, SysDataCallBack sysCallBack)
{
    if (!callBack || !sysCallBack){
        printf("回调函数不能为nil\n");
        return false;
    }
    if (!serverIP){
        printf("服务端IP地址不能为nil\n");
        return false;
    }
    if(!SObject::checkIPAddress(serverIP)){
        printf("服务端IP地址不合法\n");
        return false;
    }
    
    this->releaseHandle();
    g_type = RemoteTimerSSPType;
    if (!g_remoteHandle) {
        g_remoteHandle = new SRemoteManager();
    }
    return g_remoteHandle->initBigTimeRemote(serverIP, callBack, sysCallBack);
}

#pragma mark ======初始化启动遥控器进攻时间端======
bool SLibraryEntrance::startAttackRemoteEngine(const char *serverIP, CmdCallBack callBack, SysDataCallBack sysCallBack)
{
    if (!callBack || !sysCallBack){
        printf("回调函数不能为nil\n");
        return false;
    }
    if (!serverIP){
        printf("服务端IP地址不能为nil\n");
        return false;
    }
    if(!SObject::checkIPAddress(serverIP)){
        printf("服务端IP地址不合法\n");
        return false;
    }
    
    this->releaseHandle();
    g_type = RemoteAttackSSPType;
    if (!g_remoteHandle){
        g_remoteHandle = new SRemoteManager();
    }
    return g_remoteHandle->initAttackRemote(serverIP, callBack, sysCallBack);
}

#pragma mark 设置专业版是否允许遥控器控制
void SLibraryEntrance::setRemoteCtrlSwitch(bool isOpen)
{
    if (g_type == UnknownType) return;
    
    if (g_type == ProfessionalSSPType) {
        g_professionalHandle->remoteCtrlSwitch(isOpen);
    }
}

#pragma mark 设置主队球权
void SLibraryEntrance::setBallCtrlHome(void)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setBallControl(true);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setBallControl(true);
    } else if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeHomeTeamCtrl);
    }
}

#pragma mark 设置客队球权
void SLibraryEntrance::setBallCtrlGuest(void)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setBallControl(false);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setBallControl(false);
    } else if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeGuestTeamCtrl);
    }
}

#pragma mark 打开蜂鸣器
void SLibraryEntrance::setOpenDing(void)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setDing(true);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setDing(true);
    }
}

#pragma mark 关闭蜂鸣器
void SLibraryEntrance::setCloseDing(void)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setDing(false);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setDing(false);
    }
}

#pragma mark 设置当前节数
void SLibraryEntrance::setGameCount(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setGameCount(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setGameCount(data);
    }
}

#pragma mark 设置主队分数
void SLibraryEntrance::setHomeScore(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setHomeScore(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setHomeScore(data);
    }
}

#pragma mark 设置客队分数
void SLibraryEntrance::setGuestScore(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setGuestScore(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setGuestScore(data);
    }
}

#pragma mark 设置主队局分
void SLibraryEntrance::setHomeGameScore(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setHomeGameScore(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setHomeGameScore(data);
    }
}

#pragma mark 设置客队局分
void SLibraryEntrance::setGuestGameScore(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setGuestGameScore(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setGuestGameScore(data);
    }
}

#pragma mark 设置主队犯规数
void SLibraryEntrance::setHomeFoulCount(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setHomeFoulCount(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setHomeFoulCount(data);
    }
}

#pragma mark 设置客队犯规数
void SLibraryEntrance::setGuestFoulCount(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setGuestFoulCount(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setGuestFoulCount(data);
    }
}

#pragma mark 设置主队暂停数
void SLibraryEntrance::setHomePauseCount(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setHomePauseCount(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setHomePauseCount(data);
    }
}

#pragma mark 设置客队暂停数
void SLibraryEntrance::setGuestPauseCount(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setGuestPauseCount(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setGuestPauseCount(data);
    }
}

#pragma mark 设置进攻时间
void SLibraryEntrance::setAttackSecond(unsigned char data)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setAttackSecond(data);
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setAttackSecond(data);
    }
}

void SLibraryEntrance::setMatchTime(unsigned char minute, unsigned char second, unsigned char second10)
{
    if (g_type == UnknownType) return;
    
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setMinute(minute);
        g_simpleHandle->setSecond(second);
        g_simpleHandle->set1_10Second(second10);
        g_simpleHandle->sendData();
    } else if (g_type == ProfessionalSSPType && g_professionalHandle != NULL) {
        g_professionalHandle->setMinute(minute);
        g_professionalHandle->setSecond(second);
        g_professionalHandle->set1_10Second(second10);
        g_professionalHandle->sendData();
    }
}


#pragma mark ==========遥控器=大时间端命令接口==========
#pragma mark =====大时间启动命令=======
void SLibraryEntrance::timerStart(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeBigTimeStart);
    }
}

#pragma mark =====大时间暂停命令=======
void SLibraryEntrance::timerStop(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeBigTimeStop);
    }
}

#pragma mark =====大时间分钟加命令=======
void SLibraryEntrance::timerMinuteAdd(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeMinuteAdd);
    }
}

#pragma mark =====大时间分钟减命令=======
void SLibraryEntrance::timerMinuteReduce(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeMinuteReduce);
    }
}

#pragma mark =====大时间秒加命令=======
void SLibraryEntrance::timerSecondAdd(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeSecondAdd);
    }
}

#pragma mark =====大时间秒减命令=======
void SLibraryEntrance::timerSecondReduce(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeSecondReduce);
    }
}

#pragma mark =====1/10秒加命令=======
void SLibraryEntrance::timerMilliSecondAdd(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCode10SecondAdd);
    }
}

#pragma mark =====1/10秒减命令=======
void SLibraryEntrance::timerMilliSecondReduce(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteTimerSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCode10SecondReduce);
    }
}

#pragma mark ==========遥控器=进攻时间端命令接口==========
#pragma mark ====进攻时间启动命令====
void SLibraryEntrance::attackStart(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteAttackSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeAttackTimeStart);
    }
}

#pragma mark ====进攻时间暂停命令====
void SLibraryEntrance::attackStop(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteAttackSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeAttackTimeStop);
    }
}

#pragma mark ====进攻时间加命令====
void SLibraryEntrance::attackSecondAdd(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteAttackSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeAttackTimeAdd);
    }
}

#pragma mark ====进攻时间减命令====
void SLibraryEntrance::attackSecondReduce(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteAttackSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeAttackTimeReduce);
    }
}

#pragma mark ====进攻时间复位====
void SLibraryEntrance::attackReset(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteAttackSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeAttackTimeReset);
    }
}

#pragma mark ====进攻时间14秒复位====
void SLibraryEntrance::attack14SecondReset(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteAttackSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCode14SecondReset);
    }
}

#pragma mark ====进攻时间清零====
void SLibraryEntrance::attackZero(void)
{
    if (g_type == UnknownType) return;
    if (g_type == RemoteAttackSSPType && g_remoteHandle != NULL) {
        g_remoteHandle->sendCmdToServer(kFCodeAttackTimeZero);
    }
}


#pragma mark =====关闭机器上的大时间显示=====
void SLibraryEntrance::closeBigTimeLed(void)
{
    if (g_type == UnknownType) return;
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setCloseBigTimeLed();
    } else if(g_type == ProfessionalSSPType && g_professionalHandle != NULL){
        g_professionalHandle->setCloseBigTimeLed();
    }
}

#pragma mark =====关闭机器上的进攻时间显示====
void SLibraryEntrance::closeAttackTimeLed(void)
{
    if (g_type == UnknownType) return;
    if (g_type == SimpleSSPType && g_simpleHandle != NULL) {
        g_simpleHandle->setCloseAttackTimeLed();
    } else if(g_type == ProfessionalSSPType && g_professionalHandle != NULL){
        g_professionalHandle->setCloseAttackTimeLed();
    }
}
