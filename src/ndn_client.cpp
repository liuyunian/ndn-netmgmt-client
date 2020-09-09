#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/interest.hpp>

#include "ndn_client.h"
#include <src/ndn_client.moc>

Client::Client(const std::string & prefix):
    m_prefix(prefix),
    m_scheduler(m_face.getIoService())
{
    m_pool = ThreadPool::getInstance();
}

void Client::requestStatusInfor(){
    m_pool->enqueue([this]{
        ndn::Name interestName = ndn::Name(m_prefix).append("ndnmgmt").append("status");
        timedSendInterest(interestName, 10000);
        m_face.processEvents();
    });
}

void Client::requestTrafficInfor(){
    m_pool->enqueue([this]{
        ndn::Name interestName = ndn::Name(m_prefix).append("ndnmgmt").append("capture-start");
        sendInterest(interestName);
        m_face.processEvents();
    });
}

void Client::stopRequestTrafficInfor(){
    m_pool->enqueue([this]{
        ndn::Name interestName = ndn::Name(m_prefix).append("ndnmgmt").append("capture-stop");
        sendInterest(interestName);
        m_face.processEvents();
    });
}

void Client::timedSendInterest(ndn::Name & interestName, unsigned long interval){
    sendInterest(interestName);
    m_eventId = m_scheduler.schedule(ndn::time::milliseconds(interval), std::bind(&Client::timedSendInterest, this, interestName, interval));
}

void Client::sendPacketInterest(uint64_t seq){
    ndn::Name interestName = ndn::Name(m_prefix).append("ndnmgmt").append("packet").appendNumber(seq);
    sendInterest(interestName);
    ++ seq;
    if(seq == UINT64_MAX){
        seq = 0;
    }
    m_eventId = m_scheduler.schedule(ndn::time::milliseconds(500), std::bind(&Client::sendPacketInterest, this, seq));
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
    if(commandField == "status"){
        const ndn::Block & content = data.getContent();
        m_statusMaxSeq = *(content.value());

        sendInterest(dataName.appendNumber(0));
    }
    else if(commandField == "capture-start"){
        const ndn::Block & content = data.getContent();
        std::string deviceInfor(reinterpret_cast<const char*>(content.value()), content.value_size());
        emit displayDeviceInfor(QString::fromStdString(deviceInfor));

        sendPacketInterest(0);
    }
    else if(commandField == "capture-stop"){
        // TODO: null
    }
    else{
        commandField = dataName.at(-2).toUri();
        if(commandField == "status"){
            const auto receiveSeq = static_cast<u_int64_t>(dataName.at(-1).toNumber());
            const ndn::Block & content = data.getContent();
            m_statusInfor.append(std::string(reinterpret_cast<const char*>(content.value()), content.value_size()));
            if((receiveSeq+1) == m_statusMaxSeq ){
                // std::cout << m_statusInfor << std::endl;
                emit displayStatsInfor(QString::fromStdString(m_statusInfor));
                m_statusInfor = "";
            }
            else{
                sendInterest(dataName.getPrefix(-1).appendNumber(receiveSeq+1));
            }
        }
        else if(commandField == "packet"){
            const ndn::Block & content = data.getContent();
            std::string packetInfor(reinterpret_cast<const char*>(content.value()), content.value_size());
            if(!packetInfor.empty()){
                // std::cout << packetInfor << std::endl;
                emit displayPacketInfor(QString::fromStdString(packetInfor));
            }
        }
        else{
            std::cerr << "no match function to deal with this data" << std::endl;
        }
    }
}

void Client::onNack(const ndn::Interest & interest){
    std::cerr << "Nack: " << interest.getName() << std::endl;
}

void Client::onTimeOut(const ndn::Interest & interest){
    std::cerr << "Time out: " << interest.getName() << std::endl;
}