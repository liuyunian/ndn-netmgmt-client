#include "respond_thread.hpp"

RespondThread::RespondThread(std::shared_ptr<ndn::Face> face, ndn::Name & name):
    r_face(face),
    r_dataName(name)
{}

void RespondThread::startRespond(){
    std::string clientCommand = r_dataName.at(-1).toUri();
    if(clientCommand == "route"){
        processRouteInformationRequest();
    }
    else if(clientCommand == "CS"){
        processCSInformationRequest();
    }
    else{
        std::cout << "no match operator" << std::endl;
    }
}

void RespondThread::processRouteInformationRequest(){
    char * statusInfor = new char[ALL_CONTENT_LENGTH];
    getNFDInformation(statusInfor);

    std::string statusInfor_string(statusInfor);
    delete [] statusInfor;
    
    int start = statusInfor_string.find("<fib>");
    int length = statusInfor_string.find("<cs>") - start;
    std::string routeInfor = "<routeInfor>" + statusInfor_string.substr(start, length) + "</routeInfor>";

    createAndSendData(routeInfor);
}

void RespondThread::processCSInformationRequest(){
    char * statusInfor = new char[ALL_CONTENT_LENGTH];
    getNFDInformation(statusInfor);

    std::string statusInfor_string(statusInfor);
    delete [] statusInfor;

    int start = statusInfor_string.find("<cs>"); 
    int length = statusInfor_string.find("<strategyChoices>") - start;
    std::string CSInfor = "<CSInfor>" + statusInfor_string.substr(start, length) + "</CSInfor>";

    createAndSendData(CSInfor);
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

void RespondThread::createAndSendData(std::string & dataContent){
    auto data = std::make_shared<ndn::Data>(r_dataName);
    data->setFreshnessPeriod(ndn::time::milliseconds(2000)); //Data包生存期1s
    data->setContent((const uint8_t *)&dataContent[0], dataContent.size());
    r_keyChain.sign(*data, ndn::signingWithSha256());
    r_face->put(*data);
}

