#include "display_status.hpp"
#include "ui_display_status.h"
#include <src/client/ui/display_status.moc>

#include "node_status.hpp"
#include "ui_node_status.h"

#include "node_entry.hpp"

#include <iostream>

DisplayStatus::DisplayStatus(std::shared_ptr<Client> & client, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayStatus),
    d_client(client)
{
    ui->setupUi(this);
    for(std::shared_ptr<NodeEntry> &entry : d_client->m_nodeEntryList){
        //TODO：填充数据
        entry->m_nodeStatus->setTitle(QString::fromStdString(entry->getNodeName() + ":" + entry->getNodePrefix().toUri()));
        this->ui->verticalLayout->addWidget(entry->m_nodeStatus);
    }
}

DisplayStatus::~DisplayStatus()
{
    delete ui;
}

void DisplayStatus::on_refresh_clicked(){
    // std::cout << "click" << std::endl;
    d_client->startRequest(); //客户端开始发送Interest包获取节点信息
}