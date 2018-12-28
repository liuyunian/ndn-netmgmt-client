#include "collect_information.hpp"
#include "ui_collect_information.h"
#include <src/client/ui/collect_information.moc>

#include "display_status.hpp"

CollectInformation::CollectInformation(std::shared_ptr<Client> & client, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CollectInformation),
    c_client(client)
{
    ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize); //设置为固定的布局,鼠标不能进行拖拽
}

CollectInformation::~CollectInformation()
{
    delete ui;
}

void CollectInformation::on_AddNodeInfor_clicked(){
    QString nodeName = this->ui->NodeName->text();
    QString nodePrefix = this->ui->NodePrefix->text();
    if(nodeName.isEmpty() || nodePrefix.isEmpty()){
        return;
    }
    c_client->m_nodeEntryList.push_back(std::make_shared<NodeEntry>(nodeName.toStdString(), nodePrefix.toStdString()));
    this->ui->NodeInforDisplay->append(nodeName + ": " + nodePrefix);
    this->ui->NodeName->clear();
    this->ui->NodePrefix->clear();
}

void CollectInformation::on_ButtonBox_accepted(){
   if(c_client->m_nodeEntryList.empty()){ //没有输入任何节点信息就点击了确认按钮
       this->ui->NodeInforDisplay->append("please add some node information!");
   }
   else{
        c_client->start(); //客户端开始发送Interest包获取节点信息
        DisplayStatus * display = new DisplayStatus(c_client);
        display -> showMaximized(); //窗口最大化显示
        this->close(); //关闭窗口
   }
}

void CollectInformation::on_ButtonBox_rejected(){
    //TODO：暂时还没想怎么处理
    this->ui->NodeInforDisplay->append("click cancle button");
}
