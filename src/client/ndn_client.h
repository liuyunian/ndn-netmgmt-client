#ifndef NDN_CLIENT_H_
#define NDN_CLIENT_H_
#include <iostream>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <QObject>
#include <QString>

#include "threadpool.h"

class Client : public QObject{
    Q_OBJECT

public:
    Client(const std::string & prefix);

    ~Client(){}

    void requestRouteInformation();

    void requestCSInformation();

    void requestCaptureInformation();

signals:
    void displayRouteInfor(QString);

    void displayCSInfor(QString);
    void displayPacketInfor(QString);

public slots: 
    void on_stopCapture();

private:
    /**
     * @brief 定时发送Interest
     * @param 参数interestName: Interest包的Name
     * @param 参数interval: 发送两次Interest包的时间间隔
     * 
    */
    void timedSendInterest(ndn::Name & interestName, unsigned long interval);

    /**
     * @brief 发送单个Interest
     * @param 参数interestName: Interest包的Name
    */
    void sendInterest(ndn::Name & interestName);
    
    void onData(const ndn::Data &data);
    void onNack(const ndn::Interest & interest);
    void onTimeOut(const ndn::Interest & interest);

private:
    ndn::Face m_face;
    ndn::util::scheduler::Scheduler m_scheduler;
    ndn::util::scheduler::EventId m_eventId;

    std::string m_prefix;
    ThreadPool * m_pool;
};

#endif // NDN_CLIENT_H_