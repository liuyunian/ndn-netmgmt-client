#include <iostream>

#include "ndn_producer.hpp"

Producer::Producer(ndn::Face& face, ndn::KeyChain& keyChain)
:m_dataContent(nullptr),
m_face(face),
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

void Producer::getAllInformation(){
    std::string cmd = "nfdc status report xml";
    FILE * ptr; //文件指针
    if((ptr=popen(cmd.c_str(), "r"))!=NULL)   
    {   
        fgets(m_dataContent, ALL_CONTENT_LENGTH, ptr); //将全部的信息都存到m_dataContent中
        std::cout << m_dataContent << std::endl;
    }   
    else  
    {   
        std::cout << "popen" << cmd << "fail" << std::endl;
    }
    pclose(ptr);   
    ptr = NULL; 
}

void Producer::getCSInformation(){
    std::string cmd = "nfdc cs info";
    char tmp[128];
    FILE * ptr; //文件指针
    if((ptr=popen(cmd.c_str(), "r"))!=NULL)   
    {   
        while(fgets(tmp, 128, ptr)!=NULL) //因为是test格式的存在换行，所以用循环拿到所有数据
        {   
           strcat(m_dataContent, tmp);
        } 
        std::cout << m_dataContent << std::endl;
    }   
    else  
    {   
        std::cout << "popen" << cmd << "fail" << std::endl;
    }
    pclose(ptr);   
    ptr = NULL;
}

void Producer::onInterest(const ndn::Interest & interest){
    ndn::Name dataName(interest.getName());
    //std::cout << dataName << std::endl;
    std::string option = dataName.at(-1).toUri();
    //std::cout << option << std::endl;
    if(option == "dataset"){
        m_dataContent = new char [ALL_CONTENT_LENGTH]; //动态分配10KB空间
        getAllInformation();
        createAndSendData(dataName);
    }
    else if(option == "cs"){
        m_dataContent = new char [PART_CONTENT_LENGTH]; //动态分配1024字节空间
        getCSInformation();
        createAndSendData(dataName);
    }
    else{
        std::cout << "no match" << std::endl;
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
    std::cout << strlen(m_dataContent) << std::endl;
    data->setContent((const uint8_t *)m_dataContent, strlen(m_dataContent));
    delete [] m_dataContent; //释放内存
    m_keyChain.sign(*data, ndn::signingWithSha256());
    m_face.put(*data);
}