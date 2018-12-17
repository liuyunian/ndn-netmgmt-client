#ifndef NDN_CLIENT_H_
#define NDN_CLIENT_H_
#include <iostream>

#include <boost/asio.hpp> //boost库中的asio库，主要用于异步IO

#include <ndn-cxx/face.hpp> //ndn-cxx face模块

#include "node_entry.hpp"

class Client
{
public:
    //std::vector<std::shared_ptr<NodeEntry>> m_nodeEntries; //存放拓扑中所有节点信息的vector
    std::list<std::shared_ptr<NodeEntry>> m_nodeEntryList; //Entry智能指针类型的列表

private:
    //ndn::Name m_prefix;
    ndn::Face& m_face; //接口

public: 
    /* @brief 构造函数*/
    Client(ndn::Face& face);
    //void setPrefix(const std::string &prefix);
    void start();

private: //内部调用的函数，私有
    void createAndSendInterest();
    void onData(const ndn::Data &data);
    void onNack();
    void onTimeOut();
};
#endif