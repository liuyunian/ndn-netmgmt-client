#include "cs_information.hpp"
#include "ui_cs_information.h"
#include <src/client/ui/cs_information.moc>

#include <iostream>
#include <thread>
#include <QDomDocument>

CSInformation::CSInformation(std::string & prefix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSInformation),
    c_CSModel(new QStandardItemModel()),
    c_prefix(prefix)
{
    ui->setupUi(this);

    c_CSModel->setColumnCount(4);
    c_CSModel->setHorizontalHeaderLabels(QStringList() << "Capacity" << "Entries" << "Hits" << "Misses");
    ui->CSInfor->setModel(c_CSModel);
    ui->CSInfor->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑
    ui->CSInfor->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //设置第一列宽度为自适应

    /**
     * 创建请求CS缓存信息的线程
    */
    std::string command("CS");
    c_requestCS = new RequestThread(c_prefix, command);
    
    connect(c_requestCS, SIGNAL(displayCSInfor(QString)),
            this, SLOT(on_displayCSInfor(QString)),
            Qt::QueuedConnection);

    std::thread requestCSThread(&RequestThread::startRequest, c_requestCS);
    requestCSThread.detach();
}

CSInformation::~CSInformation()
{
    delete ui;
    delete c_CSModel;
    delete c_requestCS;
}

void CSInformation::closeEvent(QCloseEvent * event)
{
    emit closeWindow();
}

void CSInformation::on_displayCSInfor(QString CSInfor){
    parseCSInfor(CSInfor);
}

void CSInformation::parseCSInfor(QString & CSInfor_xml){
    QDomDocument doc;
    if(!doc.setContent(CSInfor_xml))
    {
        std::cout << "read xml string error" << std::endl;
        return;
    }

    QDomNode node = doc.firstChild().firstChild(); //第二级子节点
    QDomElement capacity = node.namedItem("capacity").toElement(); //capacity
    c_CSModel->setItem(0, 0, new QStandardItem(capacity.text()));
    QDomElement nEntries = node.namedItem("nEntries").toElement(); //nEntries
    c_CSModel->setItem(0, 1, new QStandardItem(nEntries.text()));
    QDomElement nHits = node.namedItem("nHits").toElement(); //nHits
    c_CSModel->setItem(0, 2, new QStandardItem(nHits.text()));
    QDomElement nMisses = node.namedItem("nMisses").toElement(); //nMisses
    c_CSModel->setItem(0, 3, new QStandardItem(nMisses.text().mid(0, (nMisses.text().length()-1))));
}
