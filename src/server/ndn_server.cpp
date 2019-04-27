#include "ndn_server.h"
#include "threadpool.h"
#include <iostream>

Server::Server(const std::string & prefix) : 
    m_prefix(prefix),
    m_mutex(new std::mutex()),
    m_pktQue(new std::queue<std::string>()){
        std::cout << m_mutex.use_count() << std::endl;
    }

void Server::run(){
    std::cout << "SERVER IS LISTEN: " << m_prefix << std::endl;

    try {
        m_face.setInterestFilter(
            ndn::Name(m_prefix),
            bind(&Server::onInterest, this, _2),
            nullptr,
            bind(&Server::onRegisterFailed, this, _1, _2)
        );

        m_face.processEvents();     
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }    
}

void Server::onInterest(const ndn::Interest & interest){
    ndn::Name interestName = interest.getName();
    std::cout << "reveive interest: " << interestName << std::endl;

    // 路由选择
    std::string clientRequest = interestName.at(-1).toUri(); //最后一部分表示客户端的请求
    if(clientRequest == "route"){
        m_pool->enqueue([this, interestName]{
            char * statusInfor = new char[ALL_CONTENT_LENGTH];
            getNFDInformation(statusInfor);

            std::string statusInfor_string(statusInfor);
            delete[] statusInfor;
            
            int start = statusInfor_string.find("<fib>");
            int length = statusInfor_string.find("<cs>") - start;
            std::string routeInfor = "<routeInfor>" + statusInfor_string.substr(start, length) + "</routeInfor>";

            sendData(interestName, routeInfor);
        });
    }
    else if(clientRequest == "CS"){
        m_pool->enqueue([this, interestName]{
            char * statusInfor = new char[ALL_CONTENT_LENGTH];
            getNFDInformation(statusInfor);

            std::string statusInfor_string(statusInfor);
            delete [] statusInfor;

            int start = statusInfor_string.find("<cs>"); 
            int length = statusInfor_string.find("<strategyChoices>") - start;
            std::string CSInfor = "<CSInfor>" + statusInfor_string.substr(start, length) + "</CSInfor>";

            sendData(interestName, CSInfor);
        });
    }
    else if(clientRequest == "capture-start"){
        m_pool->enqueue([this, interestName]{
            sendData(interestName, "");
        });

        m_capture = std::make_unique<Capture>(m_pktQue, m_mutex);
        m_pool->enqueue([this]{
            m_capture->run();
        });
    }
    else if(clientRequest == "packet"){
        m_pool->enqueue([this, interestName]{
            m_mutex->lock();
            if(!m_pktQue->empty()){
                std::string dataContent;
                if(m_pktQue->size() < 10){
                    while(!m_pktQue->empty()){
                        dataContent.append(m_pktQue->front() + '\n');
                        m_pktQue->pop();
                    }
                }
                else{
                    for(int i = 9; i >= 0; i++){
                        dataContent.append(m_pktQue->front() + '\n');
                        m_pktQue->pop();
                    }
                }
                m_mutex->unlock();
                sendData(interestName, dataContent);
            }
            else{
                m_mutex->unlock();
                sendData(interestName, "");
            }
        });
    }
    else if(clientRequest == "capture-stop"){
        m_capture.reset();

        m_pool->enqueue([this, interestName]{
            sendData(interestName, "");
        });
    }
    else{
        std::cerr << "no match route" << std::endl;
    }
}

void Server::onRegisterFailed(const ndn::Name & prefix, const std::string & reason)
{
    std::cerr << "Prefix = " << prefix << "Registration Failure. Reason = " << reason << std::endl;
}

void Server::getNFDInformation(char * statusInfor){
    const char * cmd = "nfdc status report xml";
    FILE * ptr; //文件指针
    if((ptr=popen(cmd, "r"))!=NULL)   
    {   
        fgets(statusInfor, ALL_CONTENT_LENGTH, ptr); //将全部的信息都存到临时tmp字符数组中
    }   
    else  
    {   
        std::cerr << "popen" << cmd << "fail" << std::endl;
    }
    pclose(ptr);   
    ptr = NULL;
}

void Server::sendData(const ndn::Name & dataName, const std::string & dataContent){
    auto data = std::make_shared<ndn::Data>(dataName);
    data->setFreshnessPeriod(ndn::time::milliseconds(2000)); //Data包生存期2s
    data->setContent((const uint8_t *)&dataContent[0], dataContent.size());
    m_keyChain.sign(*data, ndn::signingWithSha256());
    m_face.put(*data);
}

