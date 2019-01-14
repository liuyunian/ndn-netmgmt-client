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
    RequestThread(std::string & prefix);
    void requestRouteInformation();
    void requestCSInformation();
    void requestCaptureInformation();

signals:
    void displayRouteInfor(QString);
    void displayCSInfor(QString);
    void displayPacketInfor(QString);

    void startCaptureSuccessfully();
    void finishCapture();

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
    void onNack();
    void onTimeOut();

private:
    ndn::Face r_face;
    ndn::util::scheduler::Scheduler r_scheduler;
    ndn::util::scheduler::EventId r_eventId;
    std::string r_prefix;
};

#endif