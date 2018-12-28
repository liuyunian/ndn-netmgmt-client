#include "node_status.hpp"
#include "ui_node_status.h"

#include <src/client/ui/node_status.moc>

#include<iostream>
#include <QDomDocument>

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
    ui->FIBTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //设置第一列宽度为自适应
    ui->FIBTable->setAlternatingRowColors(true); //设置两行之间颜色不同
    ui->FIBTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑    

    n_RIBModel->setColumnCount(6); //设置RIB表格列数
    n_RIBModel->setHorizontalHeaderLabels(QStringList() << "Prefix" << "FaceId" << "Origin" << "Cost" << "ChildInherit" << "Expires in");
    ui->RIBTable->setModel(n_RIBModel);
    ui->RIBTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //设置第一列宽度为自适应
    ui->RIBTable->setAlternatingRowColors(true); //设置两行之间颜色不同
    ui->RIBTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑 

    n_CSModel->setColumnCount(4);
    n_CSModel->setRowCount(1);
    n_CSModel->setHorizontalHeaderLabels(QStringList() << "Capacity" << "Entries" << "Hits" << "Misses");
    ui->CSInfor->setModel(n_CSModel);
    ui->CSInfor->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑
    ui->CSInfor->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //设置第一列宽度为自适应
}

NodeStatus::~NodeStatus()
{
    delete ui;
    delete n_FIBModel;
    delete n_RIBModel;
    delete n_CSModel;
}

void NodeStatus::addContents(const std::string & strXML){
    QDomDocument doc;
    if(!doc.setContent(QString::fromStdString(strXML)))
    {
        std::cout << "read xml string error" << std::endl;
        return;
    }

    QDomNode node = doc.firstChild().firstChild(); //第二级子节点
    while(!node.isNull() && node.isElement())
    {
        if(node.nodeName() == "fib"){ //fib信息
            QDomNodeList fibEntrys = node.childNodes();
            n_FIBModel->setRowCount(fibEntrys.count()); //设置FIB表格行数
            for(int i = 0; i<fibEntrys.count(); i++)
            {
                QDomNode fibEntry = fibEntrys.at(i);
                if(fibEntry.isElement()){
                    QDomElement prefix = fibEntry.namedItem("prefix").toElement(); //prefix
                    // std::cout << prefix.text().toStdString() << std::endl;
                    n_FIBModel->setItem(i, 0, new QStandardItem(prefix.text())); //在FIB表格中显示prefix

                    QDomNodeList nextHops = fibEntry.namedItem("nextHops").childNodes(); //nextHops,可能存在多条
                    for(int j = 0; j<nextHops.count(); j++){
                        QDomNode nextHop = nextHops.at(j);
                        if(nextHop.isElement()){
                            QDomElement faceId = nextHop.namedItem("faceId").toElement(); //faceId
                            // std::cout << faceId.text().toStdString() << std::endl;
                            n_FIBModel->setItem(i, 1, new QStandardItem(faceId.text())); //在FIB表格中显示faceId
                            QDomElement cost = nextHop.namedItem("cost").toElement(); //cost
                            // std::cout << cost.text().toStdString() << std::endl;
                            n_FIBModel->setItem(i, 2, new QStandardItem(cost.text())); //在FIB表格中显示cost
                        }
                    }
                }
            }
        }
        else if(node.nodeName() == "rib"){ //rib信息
            QDomNodeList ribEntrys = node.childNodes();
            n_RIBModel->setRowCount(ribEntrys.count()); //设置RIB表格行数
            for(int i = 0; i<ribEntrys.count(); i++)
            {
                QDomNode ribEntry = ribEntrys.at(i);
                if(ribEntry.isElement()){
                    QDomElement prefix = ribEntry.namedItem("prefix").toElement(); //prefix
                    // std::cout << prefix.text().toStdString() << std::endl;
                    n_RIBModel->setItem(i, 0, new QStandardItem(prefix.text())); //在RIB表格中显示prefix
                    QDomNodeList routes = ribEntry.namedItem("routes").childNodes(); //routes,可能存在多条
                    for(int j = 0; j<routes.count(); j++){
                        QDomNode route = routes.at(j);
                        if(route.isElement()){
                            QDomElement faceId = route.namedItem("faceId").toElement(); //faceId
                            // std::cout << faceId.text().toStdString() << std::endl;
                            n_RIBModel->setItem(i, 1, new QStandardItem(faceId.text())); //在RIB表格中显示faceId
                            QDomElement origin = route.namedItem("origin").toElement(); //origin
                            // std::cout << origin.text().toStdString() << std::endl;
                            n_RIBModel->setItem(i, 2, new QStandardItem(origin.text())); //在RIB表格中显示origin
                            QDomElement cost = route.namedItem("cost").toElement();
                            // std::cout << cost.text().toStdString() << std::endl;
                            n_RIBModel->setItem(i, 3, new QStandardItem(cost.text())); //在RIB表格中显示cost
                            // QDomNode childInherit = route.namedItem("flags").firstChild(); //路由继承标志
                            // std::cout << childInherit.nodeName().toStdString() << std::endl;
                            n_RIBModel->setItem(i, 4, new QStandardItem("Y")); //在RIB表格中显示ChildInherit
                            n_RIBModel->setItem(i, 5, new QStandardItem("Never")); //在RIB表格中显示Expires in
                        }
                    }
                }
            }
        }
        else if(node.nodeName() == "cs"){ //cs信息
            QDomElement capacity = node.namedItem("capacity").toElement(); //capacity
            // std::cout << capacity.text().toStdString() << std::endl;
            n_CSModel->setItem(0, 0, new QStandardItem(capacity.text()));
            QDomElement nEntries = node.namedItem("nEntries").toElement(); //nEntries
            // std::cout << nEntries.text().toStdString() << std::endl;
            n_CSModel->setItem(0, 1, new QStandardItem(nEntries.text()));
            QDomElement nHits = node.namedItem("nHits").toElement(); //nHits
            // std::cout << nHits.text().toStdString();
            n_CSModel->setItem(0, 2, new QStandardItem(nHits.text()));
            QDomElement nMisses = node.namedItem("nMisses").toElement(); //nMisses
            // std::cout << nMisses.text().toStdString() << std::endl;
            n_CSModel->setItem(0, 3, new QStandardItem(nMisses.text()));
        }
        else{
            std::cout << "error" << std::endl;
        }

        node = node.nextSibling(); //下一个兄弟节点
    }
}
