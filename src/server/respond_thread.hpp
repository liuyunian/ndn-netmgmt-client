#ifndef RESPOND_THREAD_H
#define RESPOND_THREAD_H

#include <ndn-cxx/face.hpp> // include name.hpp interest.hpp interest-filter.hpp security/key-chain.hpp
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>

#include <iostream>
#include <memory>

#define ALL_CONTENT_LENGTH 1024*11 //11k
#define OUT

class RespondThread{
public:
    RespondThread(std::shared_ptr<ndn::Face> face, ndn::Name & name);
    void startRespond();

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
    void createAndSendData(std::string & dataContent);

    // @brief 从NFD状态信息中获取路由信息，并封装成data包发送
    void processRouteInformationRequest();

    // @brief 从NFD状态信息中获取CS存储信息,并封装成data包发送
    void processCSInformationRequest();
private: 
    ndn::Name r_dataName;
    std::shared_ptr<ndn::Face> r_face;
    ndn::KeyChain r_keyChain;
};

#endif