#ifndef RESPOND_THREAD_H
#define RESPOND_THREAD_H

#include <ndn-cxx/face.hpp> // include name.hpp interest.hpp interest-filter.hpp security/key-chain.hpp
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>

#include <iostream>
#include <memory>
#include <mutex>

#define ALL_CONTENT_LENGTH 1024*11 //11k
#define OUT

class RespondThread{
public:
    // @brief 构造函数
    RespondThread(std::shared_ptr<ndn::Face> face, ndn::Name & name);

    // @brief 从NFD状态信息中获取路由信息，并封装成data包发送
    void processRouteInformationRequest();

    // @brief 从NFD状态信息中获取CS存储信息,并封装成data包发送
    void processCSInformationRequest();

    /**
     * @brief 从抓包线程中获取数据包信息,并封装成data包发送
     * @param 参数packetListPtr是指向包内容的智能指针
     * @param 参数mutex是操作packetList的互斥量
    */ 
    void processPacketInformationRequest(std::shared_ptr<std::list<std::string>> packetListPtr, std::shared_ptr<std::mutex> mutex);

    // @brief 回应Ack Data包，Data包没有内容
    void sendAckData();

private:
    /**
     * @brief 利用nfdc status report xml命令，获取nfd全部状态信息，信息的格式是xml
     * @param 字符指针,指向的内存区域用于存放nfd状态信息
    */
    void getNFDInformation(OUT char * statusInfor);

    /**
     * @brief 创建Data包并发送
     * @param string类型，数据包内容
    */
    void sendData(std::string & dataContent);

    /**
     * @brief 重载sendData方法
     * 发送的Data包的内容为空
    */
    void sendData();

private: 
    ndn::Name r_dataName;
    std::shared_ptr<ndn::Face> r_face;
    ndn::KeyChain r_keyChain;
};

#endif