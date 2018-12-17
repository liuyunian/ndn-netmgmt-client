#include "node_infor.hpp"
#include "ui_node_infor.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

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
    nodeName = this->ui->nodeName->text();
    nodePrefix = this->ui->nodePrefix->text();
    if(nodeName.isEmpty() || nodePrefix.isEmpty()){
        return;
    }
    this->ui->allInfo->setPlainText(nodeName + ": " + nodePrefix);
    this->ui->nodeName->clear();
    this->ui->nodePrefix->clear();
}

void NodeInfor::on_buttonBox_accepted()
{
    //qDebug() << "点击了确认按钮";
}

void NodeInfor::on_buttonBox_rejected()
{
    //qDebug() << "点击了取消按钮";
}
