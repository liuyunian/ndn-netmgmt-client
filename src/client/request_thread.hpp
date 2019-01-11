#ifndef REQUEST_THREAD_H_
#define REQUEST_THREAD_H_
#include <iostream>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <QObject>
#include <QString>

/**
 * RequestThread请求子线程
 * 继承QObject的目的:在主线程和请求子线程之间通信用到了qt信号与槽机制
*/
class RequestThread : public QObject{
    Q_OBJECT //如果要用信号与槽机制必须要加这句话

public:
    RequestThread(std::string & prefix, std::string & command);
    void startRequest();

signals:
    void displayRouteInfor(QString);
    void displayCSInfor(QString);

private:
    void requestRouteOrCS();
    void onData(const ndn::Data &data);
    void onNack();
    void onTimeOut();

private:
    ndn::Face r_face;
    ndn::util::scheduler::Scheduler r_scheduler;

    ndn::Name r_interestName;
    std::string r_requestCommand;
};

#endif