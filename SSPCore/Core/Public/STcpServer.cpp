//
//  STcpServer.cpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#include "STcpServer.hpp"

STcpServer::STcpServer()
{
}

STcpServer::~STcpServer()
{
    closeServerSock();
}

void STcpServer::initServerAddr(int port)
{
    //等价INADDR_ANY
    in_addr_t u_ip = this->set_inet_addr("0.0.0.0");
    memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    this->set_Addr_IPV4(&m_serverAddr, u_ip, port);
}

bool STcpServer::createSock(void)
{
    if(0 > (m_serverSockfd = this->getSock_IPV4_TCP()))
        return false;
    if (this->setSockOpt_PortUnbundling(m_serverSockfd) < 0)
        return false;
    return true;
}

bool STcpServer::bindSock(void)
{
    if (this->setSockBind_IPV4(m_serverSockfd, (struct sockaddr *)&m_serverAddr) < 0)
        return false;
    return true;
}

bool STcpServer::listenSocket(void)
{
    if (this->setSocklisten(m_serverSockfd, 3) < 0)
        return false;
    return true;
}

bool STcpServer::acceptSocket(StuClientInfo *clientInfo)
{
    socklen_t sin_size = sizeof(struct sockaddr_in);
    
    clientInfo->clientSock = accept(m_serverSockfd, (struct sockaddr *)&clientInfo->clientAddr, &sin_size);
    if (clientInfo->clientSock == -1) return false;
    
    return true;
}

#pragma mark ======发送数据======
long STcpServer::sendDataToClient(StuClientInfo *clientInfo, char *buffer, long len)
{
    if (clientInfo->clientSock == -1) return -1;//socket 异常就不要再调内核API了
    return write(clientInfo->clientSock, buffer, len);
}

#pragma mark ======接收数据======
long STcpServer::readDataFormClient(StuClientInfo *clientInfo, char *buffer, long len)
{
    if (clientInfo->clientSock == -1) return -1;//socket 异常就不要再调内核API了
    return read(clientInfo->clientSock, buffer, len);
}

#pragma mark ======关闭Server端的Socket=======
void STcpServer::closeServerSock(void)
{
    if (m_serverSockfd != -1) {
        close(m_serverSockfd);
        m_serverSockfd = -1;
    }
}
