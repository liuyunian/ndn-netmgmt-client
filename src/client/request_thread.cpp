#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/interest.hpp>

#include "request_thread.hpp"
#include <src/client/request_thread.moc>

RequestThread::RequestThread(std::string & prefix): //Name of Interest is "prefix/r_requestCommand"
r_scheduler(r_face.getIoService()),
r_prefix(prefix)
{}

void RequestThread::requestRouteInformation(){
    ndn::Name interestName = ndn::Name(r_prefix).append("route");
    timedSendInterest(interestName, 5000);
    r_face.processEvents();
}

void RequestThread::requestCSInformation(){
    ndn::Name interestName = ndn::Name(r_prefix).append("CS");
    timedSendInterest(interestName, 5000);
    r_face.processEvents();
}

void RequestThread::requestCaptureInformation(){
    ndn::Name interestName = ndn::Name(r_prefix).append("capture-start");
    sendInterest(interestName);
    r_face.processEvents();
}

void RequestThread::timedSendInterest(ndn::Name & interestName, unsigned long interval){
    sendInterest(interestName);
    r_eventId = r_scheduler.scheduleEvent(ndn::time::milliseconds(interval), std::bind(&RequestThread::timedSendInterest, this, interestName, interval));
}

void RequestThread::sendInterest(ndn::Name & interestName){
    ndn::Interest interest(interestName);

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
}

void RequestThread::onData(const ndn::Data & data){
    // std::cout << "reveive Data: " << data.getName() << std::endl;
    std::string commandField = data.getName().at(-1).toUri();
    if(commandField == "route"){
        const ndn::Block & content = data.getContent();
        std::string routeInfor(reinterpret_cast<const char*>(content.value()), content.value_size());
        emit displayRouteInfor(QString::fromStdString(routeInfor));           
    }
    else if(commandField == "CS"){
        const ndn::Block & content = data.getContent();
        std::string CSInfor(reinterpret_cast<const char*>(content.value()), content.value_size());
        emit displayCSInfor(QString::fromStdString(CSInfor));
    }
    else if(commandField == "capture-start"){
        ndn::Name interestName = ndn::Name(r_prefix).append("packet");
        timedSendInterest(interestName, 3000); //2s请求一次
        emit startCaptureSuccessfully();
    }
    else if(commandField == "packet"){
        const ndn::Block & content = data.getContent();
        std::string packetInfor(reinterpret_cast<const char*>(content.value()), content.value_size());
        if(!packetInfor.empty()){
            emit displayPacketInfor(QString::fromStdString(packetInfor));
        }
    }
    else if(commandField == "capture-stop"){
        emit finishCapture();
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

// public slots
void RequestThread::on_stopCapture(){
    r_scheduler.cancelEvent(r_eventId); 
    ndn::Name interestName = ndn::Name(r_prefix).append("capture-stop");
    sendInterest(interestName);
    r_face.processEvents();
}