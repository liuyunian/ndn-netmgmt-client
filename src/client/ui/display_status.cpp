#include "display_status.hpp"
#include "ui_display_status.h"
#include <src/client/ui/display_status.moc>

#include "node_status.hpp"
#include "ui_node_status.h"

#include "node_entry.hpp"

#include <iostream>

DisplayStatus::DisplayStatus(std::shared_ptr<Management> management, 
                            std::shared_ptr<RequestThread> request, 
                            QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayStatus),
    d_management(management)
{
    ui->setupUi(this);

    /**
     * 当请求线程收到数据包并将节点状态信息存储在NodeEntry对象之后会触发显示
     * 信号的发送者：request, 接受者：this
    */
    connect(request.get(), SIGNAL(displayNodeStatus(int)), 
            this, SLOT(onDisplayStatus(int)), 
            Qt::QueuedConnection); 

    /**
     * 当点击refresh按钮之后，触发请求线程立即请求节点状态
     * 信号的发送者：this, 接受者：request
    */
    
    connect(this, SIGNAL(requestImmediately()), 
            request.get(), SLOT(onRequestImmediately()), 
            Qt::QueuedConnection); 

    for(std::shared_ptr<NodeEntry> &entry : d_management->m_nodeEntryList){
        std::shared_ptr<NodeStatus> nodeStatus = std::make_shared<NodeStatus>();
        nodeStatus -> setTitle(QString::fromStdString(entry->getNodeName() + ":" + entry->getNodePrefix().toUri()));
        entry->setNodeStatusDisplay(nodeStatus);
        this->ui->verticalLayout->addWidget(nodeStatus.get());
    }
}

DisplayStatus::~DisplayStatus()
{
    delete ui;
}

void DisplayStatus::on_refresh_clicked(){
    emit requestImmediately();
}

/**
 * TODO: 信号与槽传递的参数最优的方式是直接传递entry指针
 * 由于qt信号与槽的机制原因，会报错
 * 这里需要进一步了解原因进行优化
*/
void DisplayStatus::onDisplayStatus(int index){
    int count = 0;
    for(std::shared_ptr<NodeEntry> &entry : d_management->m_nodeEntryList){
        count++;
        if(count == index){
            entry->getNodeStatusDisplay()->addContents(entry->getNodeStatusInfor());
        }
    }
    
}