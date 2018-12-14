#include <iostream>

#include <boost/program_options.hpp>

#include <ndn-cxx/face.hpp> //ndn-cxx face模块
#include <ndn-cxx/name.hpp> //ndn-cxx name模块

#include "ndn_client.hpp"

void usage(const boost::program_options::options_description &options){
    std::cout << "Usage: fileclient [options] <prefix> \n";
    std::cout << options;
    exit(0); //参数0表示正常退出
}

int main(int argc, char* argv[]){
    ndn::Face face;
    Client client(face);

    namespace po = boost::program_options;
    
    po::options_description visibleOptDesc("Allowed options");
    visibleOptDesc.add_options()("help,h", "print this message and exit")
                                ("prefix,p", po::value<std::string>(), "prefix and file name");

    po::variables_map optVm;    
    store(parse_command_line(argc, argv, visibleOptDesc), optVm);

    if(optVm.count("help")){
        usage(visibleOptDesc);
    }
    
    if(optVm.count("prefix")){
        client.setPrefix(optVm["prefix"].as<std::string>());
    }

    client.start();
}