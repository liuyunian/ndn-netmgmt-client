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

    void requestStatusInfor();

    void requestTrafficInfor();

    void stopRequestTrafficInfor();

signals:
    void displayStatsInfor(QString);

    void displayDeviceInfor(QString);

    void displayPacketInfor(QString);

private:
    /**
     * @brief 定时发送Interest
     * @param 参数interestName: Interest包的Name
     * @param 参数interval: 发送两次Interest包的时间间隔
     * 
    */
    void timedSendInterest(ndn::Name & interestName, unsigned long interval);

    void sendPacketInterest(uint64_t seq);

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
    ndn::scheduler::Scheduler m_scheduler;
    ndn::scheduler::ScopedEventId m_eventId;

    std::string m_prefix;
    u_int64_t m_statusMaxSeq = 0;
    std::string m_statusInfor;

    ThreadPool * m_pool;
};

#endif // NDN_CLIENT_H_