//
//  SUdpClient.cpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#include "SUdpClient.hpp"

SUdpClient::SUdpClient()
{
    m_socketfd = -1;
}

SUdpClient::~SUdpClient()
{
    closeClient();
}


bool SUdpClient::setSendAddr(const char *ip)
{
    struct in_addr u_ip;
    if (this->set_inet_aton(ip, &u_ip) == INADDR_NONE)
        return false;
    
    memset(&m_sendAddr, 0, sizeof(m_sendAddr));
    this->set_Addr_IPV4(&m_sendAddr, u_ip.s_addr, kUDPPort);
    return true;
}

void SUdpClient::setRecvBroadcastAddr(void)
{
    //表示任意地址
    in_addr_t u_ip = this->set_inet_addr("0.0.0.0");
    memset(&m_recvAddr, 0, sizeof(m_recvAddr));
    this->set_Addr_IPV4(&m_recvAddr, u_ip, kUDPPort);
}

bool SUdpClient::createSock(void)
{
    closeClient();
    
    if(-1 == (m_socketfd = this->getSock_IPV4_UDP()))
        return false;
    if(-1 == this->setSockOpt_Broadcast(m_socketfd)) {
        close(m_socketfd);
        return false;
    }
    return true;
}

bool SUdpClient::bindSock(void)
{
    if(-1 == this->setSockBind_IPV4(m_socketfd, (struct sockaddr *)&(m_recvAddr))) {
        close(m_socketfd);
        return false;
    }
    return true;
}

void SUdpClient::closeClient(void)
{
    if(m_socketfd != -1){
        close(m_socketfd);
        m_socketfd = -1;
    }
}

long SUdpClient::sendData(const void *data, int len)
{
    if (-1 == m_socketfd) return -1;
    //非阻塞-可放在主线程中
    return sendto(m_socketfd, data, len, 0,(struct sockaddr*)&m_sendAddr,sizeof(m_sendAddr));
}

long SUdpClient::recvData(unsigned char *buffer, int len, char *fromIP)
{
    struct sockaddr_in tempAddr;
    
    if (-1 == m_socketfd) return -1;
    //阻塞
    if(recvfrom(m_socketfd, buffer, len, 0, (struct sockaddr*)&tempAddr,(socklen_t*)&len) <= 0) return -1;
    strncpy(fromIP, inet_ntoa(tempAddr.sin_addr), 20);
    
    return len;
}
