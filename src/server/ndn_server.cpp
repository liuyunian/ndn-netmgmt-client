#include <iostream>
#include <thread>

#include "ndn_server.hpp"
#include "respond_thread.hpp"

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
    std::cout<<" receive interest: "<< interestName <<std::endl;

    RespondThread respond(s_face, interestName);
    std::thread respondThread(&RespondThread::startRespond, &respond);
    respondThread.detach();
}

void Server::onRegisterFailed(const ndn::Name & prefix, const std::string & reason)
{
    std::cerr << "Prefix = " << prefix << "Registration Failure. Reason = " << reason << std::endl;
}

