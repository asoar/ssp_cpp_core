//
//  SObject.cpp
//  SSPCore
//
//  Created by anpb on 2018/1/22.
//  Copyright © 2018年 www.gyound.com. All rights reserved.
//

#include "SObject.hpp"

#define IP_PATTERN "^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$"

SObject::SObject()
{
}

SObject::~SObject()
{
}

//ip为"0.0.0.0"时等价htonl(INADDR_ANY)，表示任意地址,
//一般接收数据时使用
in_addr_t SObject::set_inet_addr(const char *c_ip)
{
    return inet_addr(c_ip);//只能处理IPV4
}

in_addr_t SObject::set_inet_network(const char *c_ip)
{
    return inet_network(c_ip);//只能处理IPV4
}

int SObject::set_inet_aton(const char *c_ip, struct in_addr *u_ip)
{
    return inet_aton(c_ip, u_ip);//只能处理IPV4
}

char *SObject::set_inet_ntoa(struct in_addr ip)
{
    return inet_ntoa(ip);//只能处理IPV4
}

const char *SObject::set_inet_ntop_IPV4(const void *src_ip, char *dst_ip, socklen_t len)
{
    return inet_ntop(AF_INET, src_ip, dst_ip, len);
}

const char *SObject::set_inet_ntop_IPV6(const void *src_ip, char *dst_ip, socklen_t len)
{
    return inet_ntop(AF_INET6, src_ip, dst_ip, len);
}

int SObject::set_inet_pton_IPV4(const char *src_ip, void *dst_ip)
{
    //IPV4和IPV6都能处理，比较新的API
    return inet_pton(AF_INET, src_ip, dst_ip);
}

int SObject::set_inet_pton_IPV6(const char *src_ip, void *dst_ip)
{
    return inet_pton(AF_INET6, src_ip, dst_ip);
}


void SObject::set_Addr_IPV4(struct sockaddr_in *addr, in_addr_t u_ip, long port)
{
    addr->sin_addr.s_addr = u_ip;
    addr->sin_family = AF_INET;//IPV4
    addr->sin_port = htons(port);
}

int SObject::setSockOpt_Broadcast(int sock)
{
    const int bOptVal = 1;
    return setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&bOptVal, sizeof(bOptVal));
}

int SObject::setSockOpt_PortUnbundling(int sock)
{
    //SO_REUSEADDR是让端口释放后立即就可以被再次使用
    const int bOptVal = 1;
    return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptVal, sizeof(bOptVal));
}


int SObject::getSock_IPV4_UDP(void)
{
    //SOCK_DGRAM,是无保障的面向消息的SOCKET
    //windows平台AF_INET和PF_INET是没有区别,
    //linux/unix则与内核版本有微小区别
    return socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
}

int SObject::getSock_IPV4_TCP(void)
{
    //SOCK_STREAM是有保障的（保证数据正确传送到对方）面向连接的SOCKET
    //IPPROTO_IP    表示IP协议
    //IPPROTO_TCP   表示TCP协议
    //IPPROTO_UDP   表示UDP协议
    return socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
}

int SObject::setSockBind_IPV4(int sock, struct sockaddr *addr)
{
    return bind(sock, addr, sizeof(struct sockaddr_in));
}

int SObject::setSockConnect(int sock, struct sockaddr_in* addr)
{
    return connect(sock, (struct sockaddr*)addr, sizeof(struct sockaddr));
}

int SObject::setSocklisten(int sock, int maxConn)
{
    return listen(sock, maxConn);
}

int SObject::setSockAccept(int sock, struct sockaddr* addr)
{
    socklen_t sin_size = sizeof(struct sockaddr);
    return accept(sock, addr, &sin_size);
}


#pragma mark 校验IP是否合法
bool SObject::checkIPAddress(const char * const ipAddr)
{
    if (!ipAddr) {
        return false;
    }
    regex_t reg;
    regmatch_t match[5];
    //编译正则表达式
    int retval = regcomp(&reg, IP_PATTERN, REG_EXTENDED | REG_NEWLINE);
    //匹配正则表达式
    retval = regexec(&reg, ipAddr, sizeof match / sizeof match[0], match, 0);
    //释放正则表达式
    regfree(&reg);
    return 0==retval? true : false;
}

#pragma mark 字符串按十六进制打印
void SObject::printHex(const char *data, const long len)
{
    if (!data) {
        return;
    }
    for (int i = 0; i < len; i++) {
        printf("%X ", (unsigned char)data[i]);
    }
    printf("\n");
}

#pragma mark 计算高位(前一个校验字节)字节校验码
inline char highCheckCode(char data [], size_t packLen)
{
    char checkCode = data[0]^data[1]^data[2];
    for (int i = 3; i < packLen - 2; i++) {
        checkCode ^= data[i];
    }
    return checkCode;
}

#pragma mark 计算低位(后一个校验字节)字节校验码
inline char lowCheckCode(char data [], size_t packLen)
{
    char checkCode = data[4]^0x11;
    //取数据、与数据进行校验
    for (int i = 5; i < packLen - 7 + 5; i++) {
        checkCode ^= data[i];
    }
    return checkCode;
}

#pragma mark ===TCP交互数据组包===
char *SObject::dataPackage(unsigned char fCode, const char *src)
{
    //3字节头+1字节数据包长度+1字节功能码+2字节校验码+dataLen
    bool bZeroch = false;//是否是\0特殊字符
    size_t srcLen = 0;
    if (src) {
        srcLen = strlen(src);
        if (0 == strcmp(src, "")) {
            srcLen = 1;//处理字符0
            bZeroch = true;
        }
    }
    size_t packLen = 7 + srcLen;
    char *dst = new char[packLen+1];
    bzero(dst, packLen+1);
    
    dst[0] = kProtocolHeaderOne;
    dst[1] = kProtocolHeaderTwo;
    dst[2] = kProtocolHeaderThree;
    dst[3] = packLen;
    dst[4] = fCode;
    if (src) {//有数据则copy
        for (int i = 0,j = 5; i < srcLen; i++, j++) {
            dst[j] = src[i];
        }
    }
    if (bZeroch) {
        dst[5] = '\372';//数值是250转换为特殊字符代替0
    }
    dst[packLen - 2] = highCheckCode(dst, packLen);
    dst[packLen - 1] = lowCheckCode(dst, packLen);
    
    return dst;
}

#pragma mark ====校验TCP交互数据包合法性====
bool SObject::checkTcpDataPack(const char *data)
{
    //解析数据协议
    if ((unsigned char)data[0] != 0xEA ||
        (unsigned char)data[1] != 0xEB ||
        (unsigned char)data[2] != 0xEF) {
        //数据不合法
        return false;
    }
    
    int packLen = data[3];
    unsigned char checkCode1 = data[0] ^ data[1] ^ data[2];
    unsigned char checkCode2 = data[4]^0x11;
    for (int i = 3; i < packLen - 2; i++) {
        checkCode1 ^= data[i];
        if(i > 4) {
            checkCode2 ^= data[i];
        }
    }
    
    if(checkCode1 != (unsigned char)data[packLen-2] ||
       checkCode2 != (unsigned char)data[packLen-1]){
        //数据不合法
        return false;
    }
    
    return true;
}

bool SObject::checkUdpDataPack(unsigned char *data)
{
    //旧协议
    if (data[0] == 0xAA && data[1] == 0x55 &&
        data[2] == 0x01 && data[3] == 0xF2 &&
        data[4] == 0x00 && data[36] == 0xFA)
        return true;
    //新协议
    if (data[0] == 0xBB && data[1] == 0x33 &&
        data[2] == 0x22 && data[3] == 0xF1 &&
        data[4] == 0x00 && data[36] == 0xFC)
        return true;
    
    return false;
}
