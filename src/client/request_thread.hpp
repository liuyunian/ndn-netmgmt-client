#ifndef REQUEST_THREAD_H_
#define REQUEST_THREAD_H_
#include <iostream>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <QObject>

#include "ndn_management.hpp"

/**
 * RequestThread请求子线程
 * 继承QObject的目的:在主线程和请求子线程之间通信用到了qt信号与槽机制
*/
class RequestThread : public QObject{
    Q_OBJECT //如果要用信号与槽机制必须要加这句话

public:
    RequestThread(std::shared_ptr<Management> management);
    void startRequest();

signals:
    void displayNodeStatus(int index);

public slots:
    void onRequestImmediately();

private:
    void createAndSendInterest();
    void onData(const ndn::Data &data);
    void onNack();
    void onTimeOut();

private:
    ndn::Face r_face;
    ndn::util::scheduler::Scheduler r_scheduler;
    ndn::util::scheduler::EventId r_nextRequestEventId; // 下一次请求事件ID

    std::shared_ptr<Management> r_management;
    bool r_canRequestImmediately;
};

#endif