#include <iostream>

//boost
#include <boost/program_options.hpp>

//ndn
#include <ndn-cxx/face.hpp> //ndn-cxx face模块
#include <ndn-cxx/name.hpp> //ndn-cxx name模块

//qt
#include <QApplication>
#include <QLabel>

#include "ndn_client.hpp"
#include "node_infor.hpp"

void usage(const boost::program_options::options_description &options){
    std::cout << "Usage: fileclient [options] <prefix> \n";
    std::cout << options;
    exit(0); //参数0表示正常退出
}

int main(int argc, char* argv[]){
    ndn::Face face;
    //Client client(face); //创建client实例
    std::shared_ptr<Client> client = std::make_shared<Client>(face);

    QApplication app(argc, argv);
    NodeInfor w;
    w.setPointToClient(client);
    w.show(); //显示节点信息输入窗口

    /**
     * 命令行输入形式采集节点信息（节点名称+节点前缀）
    */
    // namespace po = boost::program_options;
    
    // po::options_description visibleOptDesc("Allowed options");
    // visibleOptDesc.add_options()("help,h", "print help information and exit")
    //                             ("prefix,p", po::value<std::vector<std::string>>(), "a list of node prefix");

    // po::variables_map optVm;    
    // store(parse_command_line(argc, argv, visibleOptDesc), optVm);

    // if(optVm.count("help")){
    //     usage(visibleOptDesc);
    // }
    
    // if(optVm.count("prefix")){
    //     for(auto & nodeInfo : optVm["prefix"].as<std::vector<std::string>>()){
    //         //分割字符串
    //         int index = nodeInfo.find(':');
    //         std::string nodeName = nodeInfo.substr(0, index);
    //         std::string nodePrefix = nodeInfo.substr(index+1, (nodeInfo.size()-index-1));
    //         // std::cout << nodeName << std::endl;
    //         // std::cout << nodePrefix << std::endl;
    //         //产生对应的NodeEntry对象，并将指针存到m_nodeEntryList中
    //         client.m_nodeEntryList.push_back(std::make_shared<NodeEntry>(nodeName, nodePrefix));
    //     }
    // }

    // client->start();
    
    return app.exec();
}