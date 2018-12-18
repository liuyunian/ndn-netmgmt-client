#include "node_infor.hpp"
#include "ui_node_infor.h"
#include <src/client/node_infor.moc>

NodeInfor::NodeInfor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NodeInfor)
{
    ui->setupUi(this);
}

NodeInfor::~NodeInfor()
{
    delete ui;
}

void NodeInfor::on_addNodeInfo_clicked()
{
    QString nodeName = this->ui->nodeName->text();
    QString nodePrefix = this->ui->nodePrefix->text();
    if(nodeName.isEmpty() || nodePrefix.isEmpty()){
        return;
    }
    u_client->m_nodeEntryList.push_back(std::make_shared<NodeEntry>(nodeName.toStdString(), nodePrefix.toStdString()));
    this->ui->allInfo->append(nodeName + ": " + nodePrefix);
    this->ui->nodeName->clear();
    this->ui->nodePrefix->clear();
}

void NodeInfor::on_buttonBox_accepted()
{
    if(u_client->m_nodeEntryList.empty()){ //没有输入任何节点信息就点击了确认按钮
        this->ui->allInfo->append("please add some node information!");
    }
    else{
        u_client->start(); //客户端开始发送Interest包获取节点信息
        this->close(); //关闭窗口
    }
}

void NodeInfor::on_buttonBox_rejected()
{
    //TODO：暂时还没想怎么处理
    this->ui->allInfo->append("click cancle button");
}
