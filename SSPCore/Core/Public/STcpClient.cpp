//
//  STcpClient.cpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#include "STcpClient.hpp"

STcpClient::STcpClient()
{
    m_socketfd = -1;
}

STcpClient::~STcpClient()
{
    closeSock();
}

#pragma mark 初始化TCP客户端地址
bool STcpClient::initClientAddr(const char *serverIP, int port)
{
    memset(&m_sockAddr, 0, sizeof(m_sockAddr));
    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(kTCPPort);
    if (inet_pton(AF_INET, serverIP, &m_sockAddr.sin_addr.s_addr) <= 0) {
        printf("inet_pton Error!");
        return false;
    }
    
    return true;
}

#pragma mark 创建TCP Socket
bool STcpClient::createSock(void)
{
    if (m_socketfd != -1) closeSock();
    
    if ((m_socketfd = this->getSock_IPV4_TCP()) < 0)
        return false;
    if (this->setSockOpt_PortUnbundling(m_socketfd) < 0)
        return false;
    return true;
}

#pragma mark 连接服务器
bool STcpClient::connServer(void)
{
    if (this->setSockConnect(m_socketfd, &m_sockAddr) < 0)
        return false;
    return true;
}

#pragma mark 关闭TCP Socket
void STcpClient::closeSock(void)
{
    if (m_socketfd != -1) {
        close(m_socketfd);
        m_socketfd = -1;
    }
}

#pragma mark 向服务器发送数据
long STcpClient::sendDataToServer(const char *data, long len)
{
    if (m_socketfd == -1)
        return -1;
    return write(m_socketfd, data, len);
}

#pragma mark 接收服务器发来的数据
long STcpClient::recvDataFromServer(char *data, long len)
{
    if (m_socketfd == -1)
        return -1;
    return read(m_socketfd, data, len);
}
