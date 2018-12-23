#include <iostream>

#include "ndn_producer.hpp"

Producer::Producer(ndn::Face& face, ndn::KeyChain& keyChain)
:m_face(face),
m_keyChain(keyChain)
{}

void Producer::setPrefix(const std::string &prefix){
    //std::cout << prefix << std::endl;
    m_prefix = ndn::Name(prefix);
}

void Producer::start(){
    std::cout << "SERVER IS LISTEN: " << m_prefix << std::endl;

    try {
        m_registeredPrefix = m_face.setInterestFilter(
            m_prefix,
            bind(&Producer::onInterest, this, _2),
            nullptr,
            bind(&Producer::onRegisterFailed, this, _1, _2)
        );

        m_face.processEvents();     
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }    
}

void Producer::getNFDInformation(){
    std::string cmd = "nfdc status report xml";
    char tmp[ALL_CONTENT_LENGTH];
    FILE * ptr; //文件指针
    if((ptr=popen(cmd.c_str(), "r"))!=NULL)   
    {   
        fgets(tmp, ALL_CONTENT_LENGTH, ptr); //将全部的信息都存到临时tmp字符数组中
        //std::cout << tmp << std::endl;
    }   
    else  
    {   
        std::cout << "popen" << cmd << "fail" << std::endl;
    }
    pclose(ptr);   
    ptr = NULL;

    std::string tmp_str(tmp); //转化成string,容易进行操作
    int start = tmp_str.find("<fib>"); 
    m_dataContent = "<nodeStatus>" + 
                    tmp_str.substr(start, (tmp_str.find("<strategyChoices>") - start)) +
                    "</nodeStatus>";
    // std::cout << m_dataContent <<std::endl;
    // std::cout << m_dataContent.size() <<std::endl;
}

void Producer::onInterest(const ndn::Interest & interest){
    ndn::Name dataName(interest.getName());
    std::cout<<" receive interest: "<< dataName <<std::endl;
    m_dataContent = new char[PART_CONTENT_LENGTH]; //动态分配1024字节空间
    getNFDInformation();
    createAndSendData(dataName);
}

void Producer::onRegisterFailed(const ndn::Name& prefix, const std::string& reason)
{
    std::cerr << "Prefix = " << prefix << "Registration Failure. Reason = " << reason << std::endl;
}

void Producer::createAndSendData(const ndn::Name &dataName){
    auto data = std::make_shared<ndn::Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(ndn::time::milliseconds(1000)); //Data包生存期1s
    data->setContent((const uint8_t *)&m_dataContent[0], m_dataContent.size());
    m_keyChain.sign(*data, ndn::signingWithSha256());
    m_face.put(*data);
}