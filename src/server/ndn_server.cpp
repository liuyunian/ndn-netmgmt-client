#include "ndn_server.hpp"
#include "respond_thread.hpp"

#include <thread>

void Server::run(){
    std::cout << "SERVER IS LISTEN: " << s_prefix << std::endl;

    try {
        s_face->setInterestFilter(
            ndn::Name(s_prefix),
            bind(&Server::onInterest, this, _2),
            nullptr,
            bind(&Server::onRegisterFailed, this, _1, _2)
        );

        s_face->processEvents();     
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }    
}

void Server::onInterest(const ndn::Interest & interest){
    ndn::Name interestName = interest.getName();
    // std::cout << "reveive interest: " << interestName << std::endl;

    // 路由选择
    std::string clientCommand = interestName.at(-1).toUri();
    if(clientCommand == "route"){
        RespondThread respond(s_face, interestName);
        std::thread respondThread(&RespondThread::processRouteInformationRequest, &respond);
        respondThread.detach();
    }
    else if(clientCommand == "CS"){
        RespondThread respond(s_face, interestName);
        std::thread respondThread(&RespondThread::processCSInformationRequest, &respond);
        respondThread.detach();
    }
    else if(clientCommand == "capture-start"){
        s_capture = new NdnCapture(s_packetListPtr, s_mutex);
        std::thread captureThread(&NdnCapture::run, s_capture); //创建抓包线程
        captureThread.detach();

        /**
         * 这里这样直接起两个线程处理是有问题的
         * 正确的做法是captureThread线程抓包的条件初始化完成之后，触发回应Data
        */
        RespondThread respond(s_face, interestName);
        std::thread respondThread(&RespondThread::sendAckData, &respond); //创建回应线程
        respondThread.detach();
    }
    else if(clientCommand == "packet"){
        RespondThread respond(s_face, interestName);
        std::thread respondThread(&RespondThread::processPacketInformationRequest, &respond, s_packetListPtr, s_mutex); //创建回应线程
        respondThread.detach();
    }
    else if(clientCommand == "capture-stop"){
        delete s_capture; //释放捕获数据包进程对象的内存，将停止捕获数据包

        /**
         * 这里这样直接起一个线程发送回应Data是有问题的
         * 正确的做法是captureThread线程销毁之后，触发回应Data
        */
        RespondThread respond(s_face, interestName);
        std::thread respondThread(&RespondThread::sendAckData, &respond); //创建回应线程
        respondThread.detach();
    }
    else{
        std::cerr << "no match route" << std::endl;
    }
}

void Server::onRegisterFailed(const ndn::Name & prefix, const std::string & reason)
{
    std::cerr << "Prefix = " << prefix << "Registration Failure. Reason = " << reason << std::endl;
}

