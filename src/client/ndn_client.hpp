#ifndef NDN_CLIENT_H_
#define NDN_CLIENT_H_
#include <iostream>
// #include <boost/asio.hpp> //boost库中的asio库，主要用于异步IO

#include <ndn-cxx/face.hpp> //ndn-cxx face模块
// #include <ndn-cxx/util/scheduler.hpp> //ndn-cxx 调度模块
// #include <ndn-cxx/util/scheduler-scoped-event-id.hpp> //ndn-cxx 调度范围事件

#include "node_entry.hpp"

class Client
{
public:
    std::list<std::shared_ptr<NodeEntry>> m_nodeEntryList; //Entry智能指针类型的列表

private:
    ndn::Face& m_face; //接口
    // ndn::util::scheduler::Scheduler m_scheduler; //调度
    // ndn::util::scheduler::ScopedEventId m_nextRequestEvent; //下一次请求节点状态事件

public: 
    /* @brief 构造函数*/
    Client(ndn::Face& face);
    void startRequest();

private: //内部调用的函数，私有
    void createAndSendInterest();
    void onData(const ndn::Data &data);
    void onNack();
    void onTimeOut();
};
#endif