#include "cs_information.h"
#include "ui_cs_information.h"
#include <src/client/ui/cs_information.moc>

#include <iostream>
#include <QDomDocument>

CSInformation::CSInformation(std::string & prefix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSInformation),
    m_CSModel(new QStandardItemModel()),
    m_prefix(prefix)
{
    ui->setupUi(this);

    m_CSModel->setColumnCount(4);
    m_CSModel->setHorizontalHeaderLabels(QStringList() << "Capacity" << "Entries" << "Hits" << "Misses");
    ui->CSInfor->setModel(m_CSModel);
    ui->CSInfor->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑
    ui->CSInfor->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //设置第一列宽度为自适应

    m_client = std::make_unique<Client>(m_prefix);
    connect(m_client.get(), SIGNAL(displayCSInfor(QString)),
            this, SLOT(on_displayCSInfor(QString)),
            Qt::QueuedConnection);
    m_client->requestCSInformation();
}

CSInformation::~CSInformation()
{
    delete ui;
    delete m_CSModel;
}

void CSInformation::closeEvent(QCloseEvent * event)
{
    emit closeWindow();
}

void CSInformation::on_displayCSInfor(QString CSInfor){
    QDomDocument doc;
    if(!doc.setContent(CSInfor))
    {
        std::cout << "read xml string error" << std::endl;
        return;
    }

    QDomNode node = doc.firstChild().firstChild(); //第二级子节点
    QDomElement capacity = node.namedItem("capacity").toElement(); //capacity
    m_CSModel->setItem(0, 0, new QStandardItem(capacity.text()));
    QDomElement nEntries = node.namedItem("nEntries").toElement(); //nEntries
    m_CSModel->setItem(0, 1, new QStandardItem(nEntries.text()));
    QDomElement nHits = node.namedItem("nHits").toElement(); //nHits
    m_CSModel->setItem(0, 2, new QStandardItem(nHits.text()));
    QDomElement nMisses = node.namedItem("nMisses").toElement(); //nMisses
    m_CSModel->setItem(0, 3, new QStandardItem(nMisses.text().mid(0, (nMisses.text().length()-1))));
}
