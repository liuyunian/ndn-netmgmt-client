#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/interest.hpp>

#include "ndn_client.h"
#include <src/client/ndn_client.moc>

Client::Client(const std::string & prefix):
    m_prefix(prefix),
    m_scheduler(m_face.getIoService()){
        m_pool = ThreadPool::getInstance();
    }

void Client::requestRouteInformation(){
    m_pool->enqueue([this]{
        ndn::Name interestName = ndn::Name(m_prefix).append("route");
        timedSendInterest(interestName, 5000);
        m_face.processEvents();
    });
}

void Client::requestCSInformation(){
    m_pool->enqueue([this]{
        ndn::Name interestName = ndn::Name(m_prefix).append("CS");
        timedSendInterest(interestName, 5000);
        m_face.processEvents();
    });
}

void Client::requestCaptureInformation(){
    m_pool->enqueue([this]{
        ndn::Name interestName = ndn::Name(m_prefix).append("capture-start");
        sendInterest(interestName);
        m_face.processEvents();
    });
}

void Client::timedSendInterest(ndn::Name & interestName, unsigned long interval){
    sendInterest(interestName);
    m_eventId = m_scheduler.scheduleEvent(ndn::time::milliseconds(interval), std::bind(&Client::timedSendInterest, this, interestName, interval));
}

void Client::sendInterest(ndn::Name & interestName){
    ndn::Interest interest(interestName);

    interest.setCanBePrefix(false);
    interest.setMustBeFresh(true);
    interest.setInterestLifetime(ndn::time::milliseconds(2000));
    interest.setNonce(std::rand());

    try {
        m_face.expressInterest(interest,
        std::bind(&Client::onData, this, _2), 
        std::bind(&Client::onNack, this, _1), 
        std::bind(&Client::onTimeOut, this, _1));

        std::cout << "send Interest: " << interest.getName() << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void Client::onData(const ndn::Data & data){
    ndn::Name dataName = data.getName();
    std::cout << "reveive Data: " << dataName << std::endl;
    std::string commandField = dataName.at(-1).toUri(); 
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
        m_pool->enqueue([this]{
            ndn::Name interestName = ndn::Name(m_prefix).append("packet");
            timedSendInterest(interestName, 3000);
            m_face.processEvents();
        });
    }
    else if(commandField == "packet"){
        const ndn::Block & content = data.getContent();
        std::string packetInfor(reinterpret_cast<const char*>(content.value()), content.value_size());
        if(!packetInfor.empty()){
            emit displayPacketInfor(QString::fromStdString(packetInfor));
        }
    }
    else if(commandField == "capture-stop"){
        // TODO: null
    }
    else{
        std::cerr << "no match function to deal with this data" << std::endl;
    }
}

/**
 * TODO: 收到Nack包之后做何处理
*/
void Client::onNack(const ndn::Interest & interest){
    std::cerr << "Nack: " << interest.getName() << std::endl;
}

/**
 * TODO: 超时之后做何处理
*/
void Client::onTimeOut(const ndn::Interest & interest){
    std::cerr << "Time out: " << interest.getName() << std::endl;
}

// public slots
void Client::on_stopCapture(){
    m_scheduler.cancelEvent(m_eventId);

    m_pool->enqueue([this]{
        ndn::Name interestName = ndn::Name(m_prefix).append("capture-stop");
        sendInterest(interestName);
        m_face.processEvents();
    });
}