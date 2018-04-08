//
//  SObject.hpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#ifndef SObject_hpp
#define SObject_hpp

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <syslog.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "../Entrance/SLibraryEntrance.hpp"


#define kBroadcastIP "255.255.255.255"
#define kTCPPort 13245
#define kUDPPort 36879

/* TCP交互协议包
 1、协议头(3Byte):0xea,0xeb,0xef
 2、数据长度（1Byte）(整个数据包长度)
 3、功能码(1Byte)
 4、数据(nByte)
 5、校验码(2Byte)
 
 简单校验码:
 1、 高8位:所有字节异或和
 低8位:功能码、数据、0x11异或和
 */
//协议头
#define kProtocolHeaderOne     0xea
#define kProtocolHeaderTwo     0xeb
#define kProtocolHeaderThree   0xef

#define kFCodeAttackIdentity   0x60//客户端身份==进攻时间控制端
#define kFCodeTimerIdentity    0x61//客户端身份==大时间控制端

class SObject
{
public:
    SObject();
    ~SObject();
    
protected:
    /**
     *  点分十进制的IP转换成一个无符号长整数型的
     *  二进制网络字节序、IPV4地址IP（大端）不识别255.255.255.255
     *
     *  @param c_ip 将要转换的字符串点分十进制IP
     *
     *  @return 失败返回INADDR_NONE
     */
    in_addr_t set_inet_addr(const char *c_ip);
    
    /**
     *  点分十进制的IP转换成一个无符号长整数型的
     *  主机字节序IPV4的IP（小端） 不识别255.255.255.255
     *
     *  @param c_ip 将要转换的字符串点分十进制IP
     *
     *  @return 失败返回INADDR_NONE
     */
    in_addr_t set_inet_network(const char *c_ip);
    
    /**
     *  点分十进制的IP转换成一个无符号长整数型的
     *  二进制网络字节序、IPV4地址IP（大端）识别255.255.255.255
     *
     *  @param c_ip 将要转换的网络IP
     *  @param u_ip 转换的网络IP
     *
     *  @return 失败返回INADDR_NONE
     */
    int set_inet_aton(const char *c_ip, struct in_addr *u_ip);
    
    /**
     *  IPV4网络字节序无符号长整数型IP转换成字符串点分十进制IP
     *
     *  @param ip 将要转换的网络IP
     *
     *  @return 返回点分十进制IP
     */
    char *set_inet_ntoa(struct in_addr ip);
    
    /**
     *  网络字节序转换成 点分十进制的IP
     *
     *  @param src_ip 将要转换的网络字节序
     *  @param dst_ip 转换的后的点分十进制IP
     *
     *  @return ,errno设置为EAFNOSUPPORT
     */
    const char *set_inet_ntop_IPV4(const void *src_ip, char *dst_ip, socklen_t len);
    
    /**
     *  网络字节序转换成 点分十进制的IP
     *
     *  @param src_ip 将要转换的网络字节序
     *  @param dst_ip 转换的后的点分十进制IP
     *
     *  @return ,errno设置为EAFNOSUPPORT
     */
    const char *set_inet_ntop_IPV6(const void *src_ip, char *dst_ip, socklen_t len);
    
    /**
     *  IPV4  点分十进制的IP转换成 网络字节序
     *
     *  @param src_ip 将要转换的字符串点分十进制IP
     *  @param dst_ip 转换的后的IP
     *
     *  @return 失败返回负值,如果src_ip格式不是IPV4则返回0
     */
    int set_inet_pton_IPV4(const char *src_ip, void *dst_ip);
    
    /**
     *  IPV6  点分十进制的IP转换成 网络字节序
     *
     *  @param src_ip 将要转换的字符串点分十进制IP
     *  @param dst_ip 转换的后的IP
     *
     *  @return 失败返回负值,如果src_ip格式不是IPV6则返回0
     */
    int set_inet_pton_IPV6(const char *src_ip, void *dst_ip);
   
    
    /**
     *  UDP 初始化发送地址族---IPV4
     *
     *  @param addr 需要设置的地址族
     *  @param u_ip 需要设置的IP
     *  @param port 需要设置的port
     */
    void set_Addr_IPV4(struct sockaddr_in *addr, in_addr_t u_ip, long port);
    
    /**
     *  sock套接字设置为广播属性
     *
     *  @param sock  需要设置的套接字
     *
     *  @return 失败返回-1
     */
    int setSockOpt_Broadcast(int sock);
    
    /**
     *  当sock套接字端口释放后，端口可以立即被重新使用,
     *  系统默认是会过2分钟才能使用
     *
     *  @param sock  需要设置的套接字
     *
     *  @return 失败返回-1
     */
    int setSockOpt_PortUnbundling(int sock);
    
    /**
     *  获取Socket，数据报，UDP---IPV4
     *
     *  @return 失败返回-1，成功返回文件描述符
     */
    int getSock_IPV4_UDP(void);
    
    /**
     *  获取Socket，数据流，TCP---IPV4
     *
     *  @return 失败返回-1，成功返回socket
     */
    int getSock_IPV4_TCP(void);
    
    /**
     *  addr和sock绑定
     *
     *  @param sock 需要绑定的sock
     *  @param addr 需要绑定的addr
     *
     *  @return 失败返回-1
     */
    int setSockBind_IPV4(int sock, struct sockaddr *addr);
    
    /**
     *  TCP C端连接S端
     *
     *  @param sock  C端sock
     *  @param addr  S的地址配置信息
     *
     *  @return 失败返回-1
     */
    int setSockConnect(int sock, struct sockaddr_in* addr);
    
    /**
     *  TCP S端监听sock
     *
     *  @param sock     需要监听的sock
     *  @param maxConn  设置允许连接的上限客户数
     *
     *  @return 失败返回-1
     */
    int setSocklisten(int sock, int maxConn);
    
    /**
     *  TCP S端阻塞等待客户端连接
     *
     *  @param sock  S端监听的socket
     *  @param addr  缓存连接上来的C端的地址信息
     *
     *  @return 失败返回-1,成功返回文件描述符用于和C端交互
     */
    int setSockAccept(int sock, struct sockaddr* addr);
    
public:
    /**
     *  校验IP是否合法
     *
     *  @param ipAddr  待校验的IP地址
     */
    static bool checkIPAddress(const char * const ipAddr);
    /**
     *  十六进制打印数据
     *
     *  @param data  准备输出的数据
     *  @param len   数据长度
     */
    void printHex(const char  *data, const long len);
    /**
     *  TCP交互数据组包
     *      1、协议头(3Byte):0xea,0xeb,0xef
     *      2、数据长度（1Byte）(整个数据包长度)
     *      3、功能码(1Byte)
     *      4、数据(nByte)
     *      5、校验码(2Byte)
     *
     *  @param fCode 功能码
     *  @param src   源数据(扩展，若没有数据则传入NULL即可)
     *
     *  @return 返回完整的数据包(记得释放内存)
     */
    char *dataPackage(unsigned char fCode, const char *src);
    /**
     *  校验TCP交互数据包合法性
     *
     *  @param data  数据包
     *
     *  @return  合法数据包返回true
     */
    bool checkTcpDataPack(const char *data);
    /**
     *  校验UDP广播数据包合法性==兼容新协议
     *
     *  @param data  数据包
     *
     *  @return 合法数据包返回true
     */
    bool checkUdpDataPack(unsigned char *data);
};

#endif /* SObject_hpp */
