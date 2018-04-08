//
//  STcpClient.hpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#ifndef STcpClient_hpp
#define STcpClient_hpp

#include <stdio.h>
#include "SObject.hpp"

class STcpClient : public SObject
{
public:
    STcpClient();
    ~STcpClient();
public:
    /**
     *  初始化TCP客户端地址
     *
     *  @param serverIP 服务器IP地址
     *  @param port     绑定端口
     *
     *  @return 失败返回false
     */
    bool initClientAddr(const char *serverIP, int port);
    /**
     *  创建TCP Socket
     *
     *  @return 失败返回false
     */
    bool createSock(void);
    /**
     *  连接服务端
     *
     *  @return 失败返回false
     */
    bool connServer(void);
    /**
     *  关闭TCP Socket
     */
    void closeSock(void);
    /**
     *  发送数据给S端
     *
     *  @param data 待发数据
     *  @param len  待发数据长度
     *
     *  @return 失败返回false
     */
    long sendDataToServer(const char *data, long len);
    /**
     *  接收S端发来的数据==阻塞
     *
     *  @param data 数据缓存区
     *  @param len  读取多少Byte数据
     *
     *  @return 失败返回false
     */
    long recvDataFromServer(char *data, long len);
private:
    struct sockaddr_in   m_sockAddr;
    int                  m_socketfd;
};

#endif /* STcpClient_hpp */
