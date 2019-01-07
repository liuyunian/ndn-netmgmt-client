#include "collect_information.hpp"
#include "ui_collect_information.h"
#include <src/client/ui/collect_information.moc>

#include "display_status.hpp"
#include "request_thread.hpp"

#include <thread>

CollectInformation::CollectInformation(const std::shared_ptr<Management> management, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CollectInformation),
    c_management(management)
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
        this->ui->NodeInforDisplay->append("node name or prifix can't be empty");
        return;
    }

    std::shared_ptr<NodeEntry> entry = std::make_shared<NodeEntry>(nodeName.toStdString(), nodePrefix.toStdString());
    c_management->m_nodeEntryList.push_back(entry);

    this->ui->NodeInforDisplay->append(nodeName + ": " + nodePrefix);
    this->ui->NodeName->clear();
    this->ui->NodePrefix->clear();
}

void CollectInformation::on_ButtonBox_accepted(){ //点击确定按钮
   if(c_management->m_nodeEntryList.empty()){ //没有输入任何节点信息就点击了确认按钮
       this->ui->NodeInforDisplay->append("please add some node information!");
   }
   else{
        this->close(); //关闭收集节点信息的窗口
        std::shared_ptr<RequestThread> request = std::make_shared<RequestThread>(c_management);
        /**
         * 采用指针+用new创建对象的原因是：
         * 如果DisplayStatus display(c_management, request)创建的display对象的生存周期是else代码块
         * 造成的结果就是显示界面闪一下就会消失
         * 而采用指针+用new创建对象则指针变量到期之后，其所指向的内存并没有被释放掉
         * 这样显示界面就可以长久的存在
         * 
         * 这样处理好吗？有没有更好的处理方法
        */
        DisplayStatus * display = new DisplayStatus(c_management, request);
        display -> showMaximized(); //展示窗口最大化显示

        /**
         * 启动子线程请求数据
         * 为什么可以传递智能指针呢？
         */
        std::thread requestThread(&RequestThread::startRequest, request);
        requestThread.detach();
   }
}

void CollectInformation::on_ButtonBox_rejected(){ //点击取消按钮
    this->close();
}
