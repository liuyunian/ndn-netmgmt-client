#include "node_status.hpp"
#include "ui_node_status.h"

#include <src/client/ui/node_status.moc>

NodeStatus::NodeStatus(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::NodeStatus),
    n_FIBModel(new QStandardItemModel()),
    n_RIBModel(new QStandardItemModel()),
    n_CSModel(new QStandardItemModel())
{
    ui->setupUi(this);

    n_FIBModel->setColumnCount(3); //设置FIB表格列数
    n_FIBModel->setHorizontalHeaderLabels(QStringList() << "Prefix" << "FaceId" << "Cost"); //设置表头
    ui->FIBTable->setModel(n_FIBModel);
    // ui->FIBTable->setRowCount(1); //设置FIB表格行数
    // ui->FIBTable->setAlternatingRowColors(true); //设置两行之间颜色不同

    n_RIBModel->setColumnCount(6); //设置RIB表格列数
    n_RIBModel->setHorizontalHeaderLabels(QStringList() << "Prefix" << "FaceId" << "Origin" << "Cost" << "ChildInherit" << "Expires in");
    ui->RIBTable->setModel(n_RIBModel);
    // ui->FIBTable->setRowCount(1); //设置RIB表格行数
    // ui->RIBTable->setAlternatingRowColors(true); //设置两行之间颜色不同

    n_CSModel->setColumnCount(4);
    n_CSModel->setHorizontalHeaderLabels(QStringList() << "Capacity" << "Entries" << "Hits" << "Misses");
    ui->CSInfor->setModel(n_CSModel);

}

NodeStatus::~NodeStatus()
{
    delete ui;
    delete n_FIBModel;
    delete n_RIBModel;
    delete n_CSModel;
}
