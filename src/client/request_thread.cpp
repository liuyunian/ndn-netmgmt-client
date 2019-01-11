#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/interest.hpp>

#include "request_thread.hpp"
#include <src/client/request_thread.moc>

RequestThread::RequestThread(std::string & prefix, std::string & command): //Name of Interest is "prefix/r_requestCommand"
r_scheduler(r_face.getIoService()),
r_requestCommand(command)
{
    r_interestName = ndn::Name(prefix).append(command);
}

void RequestThread::startRequest(){
    if(r_requestCommand == "CS" || r_requestCommand == "route"){
        requestRouteOrCS();
    }
    r_face.processEvents();
}

void RequestThread::requestRouteOrCS(){
    ndn::Interest interest(r_interestName);
    std::cout << "send interest: " << r_interestName << std::endl;

    interest.setCanBePrefix(false);
    interest.setMustBeFresh(true);
    interest.setInterestLifetime(ndn::time::milliseconds(2000));
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

    r_scheduler.scheduleEvent(ndn::time::milliseconds(5000), std::bind(&RequestThread::requestRouteOrCS, this));
}

void RequestThread::onData(const ndn::Data &data){
    std::string dataContent((const char *)data.getContent().value()); //取出内容并转化成字符串形式
    std::string commandField = data.getName().at(-1).toUri();
    if(commandField == "route"){
        /**
        * 截取子字符串
        * 目的是去除后面的乱码, 有乱码的话没办法进行xml解析
        */
        std::string routeInfor = dataContent.substr(0, dataContent.find("</routeInfor>") + 13);
        /**
        * TODO:触发主线程解析并显示路由信息
        * 执行之前Data中的数据已经以字符串的形式存储在Node对象中
        */
        emit displayRouteInfor(QString::fromStdString(routeInfor));           
    }
    else if(commandField == "CS"){
        std::string CSInfor = dataContent.substr(0, dataContent.find("</CSInfor>") + 10);
        emit displayCSInfor(QString::fromStdString(CSInfor));
    }
    else{
        std::cerr << "no match function to deal with this data" << std::endl;
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