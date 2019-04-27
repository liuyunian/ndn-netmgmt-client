#ifndef NDN_SERVER_H_
#define NDN_SERVER_H_

#include <ndn-cxx/face.hpp> //include name.hpp interest.hpp interest-filter.hpp
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <memory>
#include <queue>
#include <mutex>

#include "threadpool.h"
#include "ndn_pacp.h"

#define ALL_CONTENT_LENGTH 1024*11 //11k

class Server{
public:
    Server(const std::string & prefix);

    void run();

    // /**
    //  * @brief 从抓包线程中获取数据包信息,并封装成data包发送
    //  * @param 参数packetListPtr是指向包内容的智能指针
    //  * @param 参数mutex是操作packetList的互斥量
    // */ 
    // void processPacketInformationRequest(std::shared_ptr<std::list<std::string>> packetListPtr, std::shared_ptr<std::mutex> mutex);


private: 
    // @brief 注册接收到Interest包之后的回调函数
    void onInterest(const ndn::Interest & interest);

    // @brief 注册前缀失败之后的回调函数
    void onRegisterFailed(const ndn::Name & prefix, const std::string & reason);

    /**
     * @brief 利用nfdc status report xml命令，获取nfd全部状态信息，信息的格式是xml
     * @param 字符指针,指向的内存区域用于存放nfd状态信息
    */
    void getNFDInformation(char * statusInfor);

    /**
     * @brief 创建Data包并发送
     * @param string类型，数据包内容
    */
    void sendData(const ndn::Name & dataName, const std::string & dataContent);

private:
    ndn::Face m_face;
    ndn::KeyChain m_keyChain;

    std::string m_prefix;
    std::unique_ptr<ThreadPool> m_pool = std::make_unique<ThreadPool>(10);

    std::unique_ptr<Capture> m_capture;
    std::shared_ptr<std::queue<std::string>> m_pktQue;
    std::shared_ptr<std::mutex> m_mutex;
};

#endif

