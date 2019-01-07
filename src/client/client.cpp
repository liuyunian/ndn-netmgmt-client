#include <iostream>
#include <QApplication>

#include "collect_information.hpp"
#include "ndn_management.hpp"

int main(int argc, char* argv[]){
    QApplication app(argc, argv);
    std::shared_ptr<Management> management = std::make_shared<Management>();

    CollectInformation collect(management);
    collect.show(); //显示节点信息输入窗口
    
    return app.exec();
}
