#ifndef NDN_PRODUCER_H_
#define NDN_PRODUCER_H_

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <memory>
#define ALL_CONTENT_LENGTH 10240
#define PART_CONTENT_LENGTH 1024

class Producer{
private:
    ndn::Face& m_face;
    ndn::Name m_prefix;
    ndn::KeyChain& m_keyChain;
    //char m_dataContent[CONTENT_LENGTH]; //TODO：改成动态数组？？
    char * m_dataContent;
    const ndn::RegisteredPrefixId * m_registeredPrefix; //在RIB中注册的路由前缀

public:
    Producer(ndn::Face& face, ndn::KeyChain& keyChain);
    void setPrefix(const std::string &prefix);
    void start();

private: 
    /**
     * @brief 利用nfdc status report命令，获取nfd全部状态信息，作为data的内容
    */
    void getAllInformation();

    /**
     * @brief 利用nfdc cs info 命令，获取到CS statistics information, 作为data的内容
    */
    void getCSInformation();

    /**
     * @brief 注册接收到Interest包之后的回调函数
    */
    void onInterest(const ndn::Interest & interest);

    /**
     * @brief 注册前缀失败之后的回调函数
     */
    void onRegisterFailed(const ndn::Name& prefix, const std::string& reason);

    /**
     * @brief 创建Data包并发送
     */
    void createAndSendData(const ndn::Name &dataName);
};

#endif