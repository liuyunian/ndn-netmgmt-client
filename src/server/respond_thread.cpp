#include "respond_thread.hpp"

RespondThread::RespondThread(std::shared_ptr<ndn::Face> face, ndn::Name & name):
    r_face(face),
    r_dataName(name)
{}

void RespondThread::processRouteInformationRequest(){
    char * statusInfor = new char[ALL_CONTENT_LENGTH];
    getNFDInformation(statusInfor);

    std::string statusInfor_string(statusInfor);
    delete [] statusInfor;
    
    int start = statusInfor_string.find("<fib>");
    int length = statusInfor_string.find("<cs>") - start;
    std::string routeInfor = "<routeInfor>" + statusInfor_string.substr(start, length) + "</routeInfor>";

    sendData(routeInfor);
}

void RespondThread::processCSInformationRequest(){
    char * statusInfor = new char[ALL_CONTENT_LENGTH];
    getNFDInformation(statusInfor);

    std::string statusInfor_string(statusInfor);
    delete [] statusInfor;

    int start = statusInfor_string.find("<cs>"); 
    int length = statusInfor_string.find("<strategyChoices>") - start;
    std::string CSInfor = "<CSInfor>" + statusInfor_string.substr(start, length) + "</CSInfor>";

    sendData(CSInfor);
}

void RespondThread::processPacketInformationRequest(std::shared_ptr<std::list<std::string>> packetListPtr, std::shared_ptr<std::mutex> mutex){
    mutex->lock();
    if(!packetListPtr->empty()){
        std::string dataContent = packetListPtr->front();
        packetListPtr->pop_front();
        mutex->unlock();
        sendData(dataContent);
    }
    else{
        mutex->unlock();
        sendData();
    }
    
}

void RespondThread::sendAckData(){
    sendData();
}

void RespondThread::getNFDInformation(OUT char * statusInfor){
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

void RespondThread::sendData(std::string & dataContent){
    auto data = std::make_shared<ndn::Data>(r_dataName);
    data->setFreshnessPeriod(ndn::time::milliseconds(2000)); //Data包生存期2s
    data->setContent((const uint8_t *)&dataContent[0], dataContent.size());
    r_keyChain.sign(*data, ndn::signingWithSha256());
    r_face->put(*data);
}

void RespondThread::sendData(){
    auto data = std::make_shared<ndn::Data>(r_dataName);
    data->setFreshnessPeriod(ndn::time::milliseconds(2000)); //Data包生存期2s
    r_keyChain.sign(*data, ndn::signingWithSha256());
    r_face->put(*data);
}



