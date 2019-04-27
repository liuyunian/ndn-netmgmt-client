#include "route_information.h"
#include "ui_route_information.h"
#include <src/client/ui/route_information.moc>

#include <iostream>
#include <thread>
#include <QDomDocument>

RouteInformation::RouteInformation(std::string & prefix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RouteInformation),
    m_FIBModel(new QStandardItemModel()),
    m_RIBModel(new QStandardItemModel()),
    m_prefix(prefix)
{
    ui->setupUi(this);

    m_FIBModel->setColumnCount(3); //设置FIB表格列数
    m_FIBModel->setHorizontalHeaderLabels(QStringList() << "Prefix" << "FaceId" << "Cost"); //设置表头
    ui->FIBTable->setModel(m_FIBModel);
    ui->FIBTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //设置第一列宽度为自适应
    ui->FIBTable->setAlternatingRowColors(true); //设置两行之间颜色不同
    ui->FIBTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑

    m_RIBModel->setColumnCount(6); //设置RIB表格列数
    m_RIBModel->setHorizontalHeaderLabels(QStringList() << "Prefix" << "FaceId" << "Origin" << "Cost" << "ChildInherit" << "Expires in");
    ui->RIBTable->setModel(m_RIBModel);
    ui->RIBTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //设置第一列宽度为自适应
    ui->RIBTable->setAlternatingRowColors(true); //设置两行之间颜色不同
    ui->RIBTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑

    m_client = std::make_unique<Client>(m_prefix);
    connect(m_client.get(), SIGNAL(displayRouteInfor(QString)),
            this, SLOT(on_displayRouteInfor(QString)),
            Qt::QueuedConnection);
    m_client->requestRouteInformation();
}

RouteInformation::~RouteInformation()
{
    delete ui;
    delete m_FIBModel;
    delete m_RIBModel;
}

void RouteInformation::closeEvent(QCloseEvent * event)
{
    emit closeWindow();
}

void RouteInformation::on_displayRouteInfor(QString routeInfor){
    QDomDocument doc;
    if(!doc.setContent(routeInfor))
    {
        std::cout << "read xml string error" << std::endl;
        return;
    }

    QDomNode node = doc.firstChild().firstChild(); //第二级子节点
    while(!node.isNull() && node.isElement())
    {
        if(node.nodeName() == "fib"){ //fib信息
            QDomNodeList fibEntrys = node.childNodes();
            m_FIBModel->setRowCount(fibEntrys.count()); //设置FIB表格行数
            for(int i = 0; i<fibEntrys.count(); i++)
            {
                QDomNode fibEntry = fibEntrys.at(i);
                if(fibEntry.isElement()){
                    QDomElement prefix = fibEntry.namedItem("prefix").toElement(); //prefix
                    m_FIBModel->setItem(i, 0, new QStandardItem(prefix.text())); //在FIB表格中显示prefix

                    QDomNodeList nextHops = fibEntry.namedItem("nextHops").childNodes(); //nextHops,可能存在多条
                    for(int j = 0; j<nextHops.count(); j++){
                        QDomNode nextHop = nextHops.at(j);
                        if(nextHop.isElement()){
                            QDomElement faceId = nextHop.namedItem("faceId").toElement(); //faceId
                            m_FIBModel->setItem(i, 1, new QStandardItem(faceId.text())); //在FIB表格中显示faceId
                            QDomElement cost = nextHop.namedItem("cost").toElement(); //cost
                            m_FIBModel->setItem(i, 2, new QStandardItem(cost.text())); //在FIB表格显示cost
                        }
                    }
                }
            }
        }

        else if(node.nodeName() == "rib"){ //rib信息
            QDomNodeList ribEntrys = node.childNodes();
            m_RIBModel->setRowCount(ribEntrys.count()); //设置RIB表格行数
            for(int i = 0; i<ribEntrys.count(); i++)
            {
                QDomNode ribEntry = ribEntrys.at(i);
                if(ribEntry.isElement()){
                    QDomElement prefix = ribEntry.namedItem("prefix").toElement(); //prefix
                    m_RIBModel->setItem(i, 0, new QStandardItem(prefix.text())); //在RIB表格中显示prefix
                    QDomNodeList routes = ribEntry.namedItem("routes").childNodes(); //routes,可能存在多条
                    for(int j = 0; j<routes.count(); j++){
                        QDomNode route = routes.at(j);
                        if(route.isElement()){
                            QDomElement faceId = route.namedItem("faceId").toElement(); //faceId
                            m_RIBModel->setItem(i, 1, new QStandardItem(faceId.text())); //在RIB表格中显示faceId
                            QDomElement origin = route.namedItem("origin").toElement(); //origin
                            m_RIBModel->setItem(i, 2, new QStandardItem(origin.text())); //在RIB表格中显示origin
                            QDomElement cost = route.namedItem("cost").toElement();
                            m_RIBModel->setItem(i, 3, new QStandardItem(cost.text())); //在RIB表格中显示cost
                            // QDomNode childInherit = route.namedItem("flags").firstChild(); //路由继承标志
                            m_RIBModel->setItem(i, 4, new QStandardItem("Y")); //在RIB表格中显示ChildInherit
                            m_RIBModel->setItem(i, 5, new QStandardItem("Never")); //在RIB表格中显示Expires in
                        }
                    }
                }
            }
        }

        else{
            std::cerr << "parse route information: no match" << std::endl;
        }

        node = node.nextSibling(); //下一个兄弟节点
    }
}
