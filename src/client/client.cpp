#include <iostream>
//boost
#include <boost/program_options.hpp>
//ndn
#include <ndn-cxx/face.hpp> //ndn-cxx face模块
#include <ndn-cxx/name.hpp> //ndn-cxx name模块
//qt
#include <QApplication>

#include "ndn_client.hpp"

#include "collect_information.hpp"
#include "display_status.hpp"

void usage(const boost::program_options::options_description &options){
    std::cout << "Usage: fileclient [options] <prefix> \n";
    std::cout << options;
    exit(0); //参数0表示正常退出
}

int main(int argc, char* argv[]){
    ndn::Face face;
    std::shared_ptr<Client> client = std::make_shared<Client>(face);

    QApplication app(argc, argv);
    CollectInformation * collect = new CollectInformation(client);
    collect->show(); //显示节点信息输入窗口
    
    return app.exec();
}