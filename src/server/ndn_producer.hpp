#ifndef NDN_PRODUCER_H_
#define NDN_PRODUCER_H_

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <memory>

class Producer{
public:
    Producer(ndn::Face& face, ndn::KeyChain& keyChain);
    void setPrefix(const std::string &prefix);
    void start();

private: 
    /**
     * @brief 利用nfdc status report xml命令，获取nfd全部状态信息，信息的格式是xml
     * @param 字符指针,指向的内存区域用于存放nfd状态信息
    */
    void getNFDInformation(char * statusInfor);

    // @brief 从NFD状态信息中获取路由信息
    void getRouteInformation();

    // @brief 从NFD状态信息中获取CS存储信息
    void getCSInformation();

    // @brief 注册接收到Interest包之后的回调函数
    void onInterest(const ndn::Interest & interest);

    // @brief 注册前缀失败之后的回调函数
    void onRegisterFailed(const ndn::Name& prefix, const std::string& reason);

    // @brief 创建Data包并发送
    void createAndSendData(const ndn::Name &dataName);

private:
    ndn::Face& m_face;
    ndn::Name m_prefix;
    ndn::KeyChain& m_keyChain;
    const ndn::RegisteredPrefixId * m_registeredPrefix; //在RIB中注册的路由前缀
    char * m_dataContent;
};

#endif