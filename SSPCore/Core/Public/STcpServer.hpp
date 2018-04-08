//
//  STcpServer.hpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#ifndef STcpServer_hpp
#define STcpServer_hpp

#include "SObject.hpp"

typedef struct clientInfo
{
    int         clientSock;
    sockaddr_in clientAddr;
} StuClientInfo;

class STcpServer : public SObject
{
public:
    STcpServer();
    ~STcpServer();
public:
    /**
     *  初始化TCP服务端地址
     *
     *  @param port 需要绑定的端口
     */
    void initServerAddr(int port);
    /**
     *  创建TCP Server Socket
     *
     *  @return 失败返回false
     */
    bool createSock(void);
    /**
     *  绑定Socket
     *
     *  @return 失败返回false
     */
    bool bindSock(void);
    /**
     *  监听Socket
     *
     *  @return 失败返回false
     */
    bool listenSocket(void);
    /**
     *  阻塞等待客户端连接
     *
     *  @return 失败返回false
     */
    bool acceptSocket(StuClientInfo *clientInfo);
    /**
     *  发送数据
     *
     *  @param clientInfo   指定发送的目的客户端
     *  @param buffer       待发送的数据
     *  @param len          数据的长度
     *
     *  @return 实际发送的数据长度
     */
    long sendDataToClient(StuClientInfo *clientInfo, char *buffer, long len);
    /**
     *  阻塞接收数据
     *
     *  @param clientInfo   指定接收的源客户端
     *  @param buffer       存放接收的数据内存
     *  @param len          指定接收数据的长度
     *
     *  @return 实际接收的数据长度
     */
    long readDataFormClient(StuClientInfo *clientInfo, char *buffer, long len);//阻塞
    /**
     *  关闭Server端的Socket
     */
    void closeServerSock(void);
private:
    struct sockaddr_in m_serverAddr;
    int                m_serverSockfd;//server Socket
};

#endif /* STcpServer_hpp */
