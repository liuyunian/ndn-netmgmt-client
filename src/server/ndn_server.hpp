#ifndef NDN_PRODUCER_H_
#define NDN_PRODUCER_H_

#include <ndn-cxx/face.hpp> //include name.hpp interest.hpp interest-filter.hpp

#include <memory>

class Server{
public:

    void setPrefix(const std::string & prefix){ s_prefix = prefix;}

    void run();

private: 
    // @brief 注册接收到Interest包之后的回调函数
    void onInterest(const ndn::Interest & interest);

    // @brief 注册前缀失败之后的回调函数
    void onRegisterFailed(const ndn::Name& prefix, const std::string& reason);

private:
    std::shared_ptr<ndn::Face> s_face = std::make_shared<ndn::Face>();
    std::string s_prefix;
};

#endif

