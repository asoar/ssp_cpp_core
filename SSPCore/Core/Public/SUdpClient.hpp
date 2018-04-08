//
//  SUdpClient.hpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#ifndef SUdpClient_hpp
#define SUdpClient_hpp

#include "SObject.hpp"

class SUdpClient : public SObject
{
public:
    SUdpClient();
    ~SUdpClient();
    
public:
    /**
     *  UDP 初始化发送数据Addr====指定目的IP
     *
     *  @param ip 目的IP，当传入IP"255.255.255.255"表示广播
     *
     *  @return 失败返回false
     */
    bool setSendAddr(const char *ip);

    /**
     *  UDP 接收广播数据初始化
     */
    void setRecvBroadcastAddr(void);
    
    /**
     *  创建Socket
     *
     *  @return false表示failed
     */
    bool createSock(void);
    
    /**
     *  绑定Socket
     *
     *  @return false表示failed
     */
    bool bindSock(void);
    
    /**
     *  关闭UDP Client Socket
     */
    void closeClient(void);
    
    /**
     *  发送数据
     *
     *  @param data 待发数据
     *  @param len  数据长度
     *
     *  @return 实际发送数据长度
     */
    long sendData(const void *data, int len);
    
    /**
     *  读取sock数据
     *
     *  @param buffer   缓存读到的数据
     *  @param len      计划读取数据的长度
     *  @param fromIP   数据源的IP
     *
     *  @return 实际读取数据长度
     */
    long recvData(unsigned char *buffer, int len, char *fromIP);
    
private:
    struct sockaddr_in  m_sendAddr;//用来广播数据
    struct sockaddr_in  m_recvAddr;//用来接收广播数据,用来bind Socket
    int                 m_socketfd;
    int                 m_addrLen;
};

#endif /* SUdpClient_hpp */
