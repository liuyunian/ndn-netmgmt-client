#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/interest.hpp>

#include "ndn_client.hpp"

Client::Client(ndn::Face& face):
m_face(face),
m_scheduler(m_face.getIoService()),
m_nextNodeEvent(m_scheduler)
{}

void Client::onData(const ndn::Data &data){
    std::cout << " recevice data " << std::endl;
    for(std::shared_ptr<NodeEntry> &entry : m_nodeEntryList){
        if(entry->getNodePrefix().equals(data.getName())){
            std::string dataContent((const char *)data.getContent().value()); //取出内容并转化成字符串形式
            /**
             * 截取子字符串
             * 目的是去除后面的乱码, 有乱码的话没办法进行xml解析
            */
            entry->m_nodeStatus->addContents(dataContent.substr(0, dataContent.find("</nodeStatus>") + 13));
        }
    }

}

void Client::onNack(){
    std::cout << " receive Nack " << std::endl;
}


void Client::onTimeOut(){
    std::cout << " time out " << std::endl;
}

void Client::createAndSendInterest(const ndn::Name & prefix){
    ndn::Interest interest(prefix); //创建Interest对象，代表一个Interest包
    std::cout<<" interest's prefix is "<<interest.getName()<<std::endl;

    interest.setCanBePrefix(false);
    interest.setMustBeFresh(true); //设置兴趣包是否允许重传
    interest.setInterestLifetime(ndn::time::milliseconds(1000)); //设置兴趣包的存活时间
    interest.setNonce(std::rand()); //设置兴趣包中的随机数

    try {
        m_face.expressInterest(interest,
        std::bind(&Client::onData, this, _2), //收到data包之后的回调函数
        std::bind(&Client::onNack, this), //收到Nack包之后的回调函数
        std::bind(&Client::onTimeOut, this)); //超时之后的回调函数
    }
    catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }    
}

void Client::scheduleNextNode(){
    for (const std::shared_ptr<NodeEntry> & entry : m_nodeEntryList){ //向每个节点都发送Interest
        std::cout << entry->getNodePrefix() << std::endl;
        createAndSendInterest(entry->getNodePrefix());
        m_nextNodeEvent = m_scheduler.scheduleEvent(ndn::time::milliseconds(100), std::bind(&Client::createAndSendInterest, this, entry->getNodePrefix()));
    }
}

void Client::start()
{
    scheduleNextNode();
    m_face.processEvents();
}