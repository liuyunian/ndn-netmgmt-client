#include <iostream>
#include <thread>

#include "ndn_producer.hpp"
#include "ndn_capture.hpp"

#define ALL_CONTENT_LENGTH 10240
#define OUT

Producer::Producer(ndn::Face& face, ndn::KeyChain& keyChain)
:m_face(face),
m_keyChain(keyChain)
{}

void Producer::setPrefix(const std::string &prefix){
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

void Producer::getNFDInformation(OUT char * statusInfor){
    const char * cmd = "nfdc status report xml";
    FILE * ptr; //文件指针
    if((ptr=popen(cmd, "r"))!=NULL)   
    {   
        fgets(statusInfor, ALL_CONTENT_LENGTH, ptr); //将全部的信息都存到临时tmp字符数组中
    }   
    else  
    {   
        std::cout << "popen" << cmd << "fail" << std::endl;
    }
    pclose(ptr);   
    ptr = NULL;
}

void Producer::getRouteInformation(){
    char * statusInfor = new char[ALL_CONTENT_LENGTH];
    getNFDInformation(statusInfor);

    std::string statusInfor_string(statusInfor);
    int start = statusInfor_string.find("<fib>"); 
    int length = statusInfor_string.find("<cs>") - start;
    std::string routeInfor = "<routeInfor>" + statusInfor_string.substr(start, length) + "</routeInfor>";
    
    m_dataContent = new char[length+1];
    strcpy(m_dataContent, routeInfor.c_str());

    delete [] statusInfor;
}

void Producer::getCSInformation(){
    char * statusInfor = new char[ALL_CONTENT_LENGTH];
    getNFDInformation(statusInfor);

    std::string statusInfor_string(statusInfor);
    int start = statusInfor_string.find("<cs>"); 
    int length = statusInfor_string.find("<strategyChoices>") - start;
    std::string CSInfor = "<CSInfor>" + statusInfor_string.substr(start, length) + "</CSInfor>";
    
    m_dataContent = new char[length+1];
    strcpy(m_dataContent, CSInfor.c_str());

    delete [] statusInfor;
}

void Producer::onInterest(const ndn::Interest & interest){
    ndn::Name interestName = interest.getName();
    std::cout<<" receive interest: "<< interestName <<std::endl;
    std::string clientCommand = interestName.at(-1).toUri();
    if(clientCommand == "route"){
        getRouteInformation();
        createAndSendData(interestName);
    }
    else if(clientCommand == "CS"){
        getCSInformation();
        createAndSendData(interestName);
    }
    else{
        std::cout << "no match operator" << std::endl;
        // std::cout << "触发开始抓包" << std::endl;
        // std::shared_ptr<NdnCapture> capture = std::make_shared<NdnCapture>();
        // std::thread captureThread(&NdnCapture::run, capture);
        // captureThread.detach();
    }
}

void Producer::onRegisterFailed(const ndn::Name& prefix, const std::string& reason)
{
    std::cerr << "Prefix = " << prefix << "Registration Failure. Reason = " << reason << std::endl;
}

void Producer::createAndSendData(const ndn::Name &dataName){
    auto data = std::make_shared<ndn::Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(ndn::time::milliseconds(1000)); //Data包生存期1s
    data->setContent((const uint8_t *)m_dataContent, strlen(m_dataContent));
    m_keyChain.sign(*data, ndn::signingWithSha256());
    m_face.put(*data);

    delete [] m_dataContent;
}