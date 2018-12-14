
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/interest.hpp>

#include "ndn_client.hpp"

Client::Client(ndn::Face& face)
: m_face(face)
{}

void Client::setPrefix(const std::string &prefix){
    m_prefix = ndn::Name(prefix);
}

void Client::onData(const ndn::Data &data){
    std::cout << " recevice data " << std::endl;
    ndn::Name dataName = data.getName();
    if(dataName.at(-1).toUri() == "dataset"){
        std::cout << "receive data for dataset" << std::endl;
        //TODO：这里会收到分段的数据，怎么处理
    }
    else{
        //const ndn::Block &dataContent = ;
        std::string dataContent((const char *)data.getContent().value()); //取出内容并转化成字符串形式
        std::cout << dataContent << std::endl;
    }
}

void Client::onNack(){
    std::cout << " receive Nack " << std::endl;
}


void Client::onTimeOut(){
    std::cout << " time out " << std::endl;
}

ndn::Interest Client::createInterest(){
    ndn::Interest interest(m_prefix); //创建Interest对象，代表一个Interest包
    std::cout<<" interest's prefix is "<<interest.getName()<<std::endl;

    interest.setCanBePrefix(false);
    interest.setMustBeFresh(true); //设置兴趣包是否允许重传
    interest.setInterestLifetime(ndn::time::milliseconds(1000)); //设置兴趣包的存活时间
    interest.setNonce(std::rand()); //设置兴趣包中的随机数

    return interest;
}

void Client::start()
{

    /*  通过传输端口发送Interest包
        读取整个响应并调用onData或者onTimeOut处理函数
    */
    try {
        m_face.expressInterest(createInterest(),
        std::bind(&Client::onData, this, _2), //收到data包之后的回调函数
        std::bind(&Client::onNack, this), //收到Nack包之后的回调函数
        std::bind(&Client::onTimeOut, this)); //超时之后的回调函数
    }
    catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }    

    m_face.processEvents();
}