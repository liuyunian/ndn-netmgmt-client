#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/interest.hpp>

#include "request_thread.hpp"
#include <src/client/request_thread.moc>

RequestThread::RequestThread(std::shared_ptr<Management> management):
r_scheduler(r_face.getIoService()),
r_management(management),
r_canRequestImmediately(false)
{}

void RequestThread::startRequest(){
    createAndSendInterest();
    r_face.processEvents();
}

void RequestThread::createAndSendInterest(){
    r_canRequestImmediately = false; //保证发送Interest包期间不能触发立刻请求事件

    for (const std::shared_ptr<NodeEntry> & entry : r_management->m_nodeEntryList){
        ndn::Interest interest(entry->getNodePrefix());

        interest.setCanBePrefix(false);
        interest.setMustBeFresh(true);
        interest.setInterestLifetime(ndn::time::milliseconds(1000));
        interest.setNonce(std::rand());

        try {
            r_face.expressInterest(interest,
            std::bind(&RequestThread::onData, this, _2), 
            std::bind(&RequestThread::onNack, this), 
            std::bind(&RequestThread::onTimeOut, this));
        }
        catch (std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
    }
    /**
     * 5秒请求一次
     * r_nextRequestEventId用于取消调度
    */
    r_nextRequestEventId = r_scheduler.scheduleEvent(ndn::time::milliseconds(5000), std::bind(&RequestThread::createAndSendInterest, this));
    r_canRequestImmediately = true; //进入调度之后可以触发立刻请求事件
}

//private slots
void RequestThread::onRequestImmediately(){
    if(r_canRequestImmediately){
        r_scheduler.cancelEvent(r_nextRequestEventId);
        std::cout << "触发立刻请求事件" << std::endl;

        /**
         * 再次执行createAndSendInterest会在r_nextRequestEventId = r_scheduler.scheduleEvent...处报错
         * 错误信息是：段错误（核心已转出）
         * 原因：无法访问r_scheduler
        */
        // createAndSendInterest();
    }
}

void RequestThread::onData(const ndn::Data &data){
    int index = 0; //用来告知主线程收到了哪个节点的状态信息
    for(std::shared_ptr<NodeEntry> &entry : r_management->m_nodeEntryList){
        index++;
        if(entry->getNodePrefix().equals(data.getName())){
            std::string dataContent((const char *)data.getContent().value()); //取出内容并转化成字符串形式
            /**
             * 截取子字符串
             * 目的是去除后面的乱码, 有乱码的话没办法进行xml解析
            */
            entry->setNodeStatusInfor(dataContent.substr(0, dataContent.find("</nodeStatus>") + 13));
            /**
             * TODO:触发主线程解析并显示数据
             * 执行之前Data中的数据已经以字符串的形式存储在NodeEntry对象中
            */
            emit displayNodeStatus(index);
            break;
        }
    }
}

/**
 * TODO: 收到Nack包之后做何处理
*/
void RequestThread::onNack(){
    std::cout << " receive Nack " << std::endl;
}

/**
 * TODO: 超时之后做何处理
*/
void RequestThread::onTimeOut(){
    std::cout << " time out " << std::endl;
}