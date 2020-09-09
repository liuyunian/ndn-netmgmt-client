#include "status_infor.h"
#include "ui_status_infor.h"
#include <src/ui/status_infor.moc>

#include <QDomDocument>

StatusInfor::StatusInfor(std::string & name, std::string & prefix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatusInfor),
    m_prefix(prefix)
{
    ui->setupUi(this);

    m_client = std::make_unique<Client>(m_prefix);
    connect(m_client.get(), SIGNAL(displayStatsInfor(QString)),
            this, SLOT(on_displayStatsInfor(QString)),
            Qt::QueuedConnection);
    m_client->requestStatusInfor();

    /* --------------------- General ------------------- */
    m_generalModel = new QStandardItemModel(this);
    m_generalModel->setColumnCount(2); // 设置表中列的个数
    m_generalModel->setHorizontalHeaderLabels(QStringList() << "Type" << "Value"); //设置表头
    m_generalModel->setItem(0, 0, new QStandardItem("Name")); 
    m_generalModel->setItem(0, 1, new QStandardItem(QString::fromStdString(name))); 
    m_generalModel->setItem(1, 0, new QStandardItem("Prefix"));
    m_generalModel->setItem(1, 1, new QStandardItem(QString::fromStdString(m_prefix)));

    ui->GeneralTable->setModel(m_generalModel);
    ui->GeneralTable->setAlternatingRowColors(true); //设置两行之间颜色不同
    ui->GeneralTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); //设置第二列宽度为自适应
    ui->GeneralTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不能编辑
    ui->GeneralTable->verticalHeader()->hide(); //隐藏序号列

    /* --------------------- Channels ------------------- */
    m_channelModel = new QStandardItemModel(this);
    m_channelModel->setColumnCount(2); 
    m_channelModel->setHorizontalHeaderLabels(QStringList() << "Channel" << "Description");

    ui->ChannelTable->setModel(m_channelModel);
    ui->ChannelTable->setAlternatingRowColors(true);
    ui->ChannelTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->ChannelTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ChannelTable->verticalHeader()->hide();

    /* --------------------- Faces ------------------- */
    m_faceModel = new QStandardItemModel(this);
    m_faceModel->setColumnCount(6); 
    m_faceModel->setHorizontalHeaderLabels(QStringList() << "Face ID" << "Remote URI" << "Local URI" << "Scope" << "Persistency" << "LinkType");

    ui->FaceTable->setModel(m_faceModel);
    ui->FaceTable->setColumnWidth(0, 50);
    ui->FaceTable->setColumnWidth(1, 300);
    ui->FaceTable->setColumnWidth(2, 300);
    ui->FaceTable->setAlternatingRowColors(true);
    ui->FaceTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->FaceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->FaceTable->verticalHeader()->hide();

    /* --------------------- FIB ------------------- */
    m_FIBModel = new QStandardItemModel(this);
    m_FIBModel->setColumnCount(3);
    m_FIBModel->setHorizontalHeaderLabels(QStringList() << "Prefix" << "FaceId" << "Cost");

    ui->FIBTable->setModel(m_FIBModel);
    ui->FIBTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->FIBTable->setAlternatingRowColors(true);
    ui->FIBTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->FIBTable->verticalHeader()->hide();

    /* --------------------- RIB ------------------- */
    m_RIBModel = new QStandardItemModel(this);
    m_RIBModel->setColumnCount(6);
    m_RIBModel->setHorizontalHeaderLabels(QStringList() << "Prefix" << "FaceId" << "Origin" << "Cost" << "ChildInherit" << "Expires in");

    ui->RIBTable->setModel(m_RIBModel);
    ui->RIBTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->RIBTable->setAlternatingRowColors(true);
    ui->RIBTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->RIBTable->verticalHeader()->hide();

    /* --------------------- Content Store ------------------- */
    m_CSModel = new QStandardItemModel(this);
    m_CSModel->setColumnCount(5);
    m_CSModel->setHorizontalHeaderLabels(QStringList() << "Capacity" << "Entries" << "Hits" << "Misses" << "Others");
    ui->CSTable->setModel(m_CSModel);
    ui->CSTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->CSTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->CSTable->verticalHeader()->hide();

    /* --------------------- Content Store ------------------- */
    m_StrategyModel = new QStandardItemModel(this);
    m_StrategyModel->setColumnCount(2);
    m_StrategyModel->setHorizontalHeaderLabels(QStringList() << "Prefix" << "Strategy Choice");
    ui->StrategyTable->setModel(m_StrategyModel);
    ui->StrategyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->StrategyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); 
    ui->StrategyTable->verticalHeader()->hide();
}

StatusInfor::~StatusInfor(){
    delete ui;
}

void StatusInfor::closeEvent(QCloseEvent * event){
    emit closeWindow();
}

void StatusInfor::on_displayStatsInfor(QString statusInfor){
    QDomDocument doc;
    if(!doc.setContent(statusInfor))
    {
        std::cout << "read xml string error" << std::endl;
        return;
    }
    else{
        QDomNode node = doc.firstChild().nextSibling().firstChild();
        while(!node.isNull() && node.isElement()){
            if(node.nodeName() == "generalStatus"){
                QDomElement elem = node.namedItem("version").toElement();
                m_generalModel->setItem(2, 0, new QStandardItem("Version"));
                m_generalModel->setItem(2, 1, new QStandardItem(elem.text()));

                elem = node.namedItem("startTime").toElement();
                m_generalModel->setItem(3, 0, new QStandardItem("Start Time"));
                m_generalModel->setItem(3, 1, new QStandardItem(elem.text()));

                elem = node.namedItem("currentTime").toElement();
                m_generalModel->setItem(4, 0, new QStandardItem("Current Time"));
                m_generalModel->setItem(4, 1, new QStandardItem(elem.text()));

                elem = node.namedItem("nNameTreeEntries").toElement();
                m_generalModel->setItem(5, 0, new QStandardItem("Name Tree Entries"));
                m_generalModel->setItem(5, 1, new QStandardItem(elem.text()));

                elem = node.namedItem("nFibEntries").toElement();
                m_generalModel->setItem(6, 0, new QStandardItem("Fib Entries"));
                m_generalModel->setItem(6, 1, new QStandardItem(elem.text()));

                elem = node.namedItem("nPitEntries").toElement();
                m_generalModel->setItem(7, 0, new QStandardItem("Pit Entries"));
                m_generalModel->setItem(7, 1, new QStandardItem(elem.text()));

                elem = node.namedItem("nMeasurementsEntries").toElement();
                m_generalModel->setItem(8, 0, new QStandardItem("Measurements Entries"));
                m_generalModel->setItem(8, 1, new QStandardItem(elem.text()));

                elem = node.namedItem("nCsEntries").toElement();
                m_generalModel->setItem(9, 0, new QStandardItem("Content Store Entries"));
                m_generalModel->setItem(9, 1, new QStandardItem(elem.text()));

                QDomNode incoming = node.namedItem("packetCounters").namedItem("incomingPackets");
                elem = incoming.namedItem("nInterests").toElement();
                m_generalModel->setItem(10, 0, new QStandardItem("Incoming Interests"));
                m_generalModel->setItem(10, 1, new QStandardItem(elem.text()));

                QDomNode outgoing = node.namedItem("packetCounters").namedItem("outgoingPackets");
                elem = outgoing.namedItem("nInterests").toElement();
                m_generalModel->setItem(11, 0, new QStandardItem("Outgoing Interests"));
                m_generalModel->setItem(11, 1, new QStandardItem(elem.text()));

                elem = incoming.namedItem("nData").toElement();
                m_generalModel->setItem(12, 0, new QStandardItem("Incoming Data"));
                m_generalModel->setItem(12, 1, new QStandardItem(elem.text()));

                elem = outgoing.namedItem("nData").toElement();
                m_generalModel->setItem(13, 0, new QStandardItem("Outgoing Data"));
                m_generalModel->setItem(13, 1, new QStandardItem(elem.text()));

                elem = incoming.namedItem("nNacks").toElement();
                m_generalModel->setItem(12, 0, new QStandardItem("Incoming Nacks"));
                m_generalModel->setItem(12, 1, new QStandardItem(elem.text()));

                elem = outgoing.namedItem("nNacks").toElement();
                m_generalModel->setItem(13, 0, new QStandardItem("Outgoing Nacks"));
                m_generalModel->setItem(13, 1, new QStandardItem(elem.text()));

                elem = node.namedItem("nSatisfiedInterests").toElement();
                m_generalModel->setItem(14, 0, new QStandardItem("Satisfied Interests"));
                m_generalModel->setItem(14, 1, new QStandardItem(elem.text()));

                elem = node.namedItem("nUnsatisfiedInterests").toElement();
                m_generalModel->setItem(15, 0, new QStandardItem("Unsatisfied Interests"));
                m_generalModel->setItem(15, 1, new QStandardItem(elem.text()));
            }
            else if(node.nodeName() == "channels"){
                QDomNodeList channels = node.childNodes();
                m_channelModel->setRowCount(channels.count());
                QDomElement uri;
                for(int i = 0; i < channels.count(); ++ i){
                    QDomNode channel = channels.at(i);
                    if(channel.isElement()){
                        uri = channel.namedItem("localUri").toElement();
                        m_channelModel->setItem(i, 0, new QStandardItem(uri.text()));
                    }
                }
            }
            else if(node.nodeName() == "faces"){
                QDomNodeList faces = node.childNodes();
                m_faceModel->setRowCount(faces.count());
                for(int i = 0; i < faces.count(); ++ i){
                    QDomNode face = faces.at(i);
                    if(face.isElement()){
                        QDomElement elem = face.namedItem("faceId").toElement();
                        m_faceModel->setItem(i, 0, new QStandardItem(elem.text()));

                        elem = face.namedItem("remoteUri").toElement();
                        m_faceModel->setItem(i, 1, new QStandardItem(elem.text()));

                        elem = face.namedItem("localUri").toElement();
                        m_faceModel->setItem(i, 2, new QStandardItem(elem.text()));

                        elem = face.namedItem("faceScope").toElement();
                        m_faceModel->setItem(i, 3, new QStandardItem(elem.text()));

                        elem = face.namedItem("facePersistency").toElement();
                        m_faceModel->setItem(i, 4, new QStandardItem(elem.text()));

                        elem = face.namedItem("linkType").toElement();
                        m_faceModel->setItem(i, 5, new QStandardItem(elem.text()));
                    }
                }

                ui->FaceTable->setColumnWidth(0, 50);
                ui->FaceTable->setColumnWidth(1, 300);
                ui->FaceTable->setColumnWidth(2, 300);
            }
            else if(node.nodeName() == "fib"){
                QDomNodeList fibEntrys = node.childNodes();
                m_FIBModel->setRowCount(fibEntrys.count());
                for(int i = 0; i < fibEntrys.count(); ++ i){
                    QDomNode fibEntry = fibEntrys.at(i);
                    if(fibEntry.isElement()){
                        QDomElement prefix = fibEntry.namedItem("prefix").toElement();
                        m_FIBModel->setItem(i, 0, new QStandardItem(prefix.text()));

                        QDomNodeList nextHops = fibEntry.namedItem("nextHops").childNodes();
                        for(int j = 0; j<nextHops.count(); j++){
                            QDomNode nextHop = nextHops.at(j);
                            if(nextHop.isElement()){
                                QDomElement faceId = nextHop.namedItem("faceId").toElement();
                                m_FIBModel->setItem(i, 1, new QStandardItem(faceId.text()));

                                QDomElement cost = nextHop.namedItem("cost").toElement();
                                m_FIBModel->setItem(i, 2, new QStandardItem(cost.text()));
                            }
                        }
                    }
                }
            }
            else if(node.nodeName() == "rib"){ 
                QDomNodeList ribEntrys = node.childNodes();
                m_RIBModel->setRowCount(ribEntrys.count()); 
                for(int i = 0; i<ribEntrys.count(); i++)
                {
                    QDomNode ribEntry = ribEntrys.at(i);
                    if(ribEntry.isElement()){
                        QDomElement prefix = ribEntry.namedItem("prefix").toElement();
                        m_RIBModel->setItem(i, 0, new QStandardItem(prefix.text()));
                        QDomNodeList routes = ribEntry.namedItem("routes").childNodes();
                        for(int j = 0; j<routes.count(); j++){
                            QDomNode route = routes.at(j);
                            if(route.isElement()){
                                QDomElement faceId = route.namedItem("faceId").toElement();
                                m_RIBModel->setItem(i, 1, new QStandardItem(faceId.text()));

                                QDomElement origin = route.namedItem("origin").toElement();
                                m_RIBModel->setItem(i, 2, new QStandardItem(origin.text()));

                                QDomElement cost = route.namedItem("cost").toElement();
                                m_RIBModel->setItem(i, 3, new QStandardItem(cost.text()));

                                // QDomNode childInherit = route.namedItem("flags").firstChild(); //路由继承标志
                                m_RIBModel->setItem(i, 4, new QStandardItem("Y"));
                                m_RIBModel->setItem(i, 5, new QStandardItem("Never"));
                            }
                        }
                    }
                }
            }
            else if(node.nodeName() == "cs"){
                QDomElement elem = node.namedItem("capacity").toElement(); //capacity
                m_CSModel->setItem(0, 0, new QStandardItem(elem.text()));
                elem = node.namedItem("nEntries").toElement(); //nEntries
                m_CSModel->setItem(0, 1, new QStandardItem(elem.text()));
                elem = node.namedItem("nHits").toElement(); //nHits
                m_CSModel->setItem(0, 2, new QStandardItem(elem.text()));
                elem = node.namedItem("nMisses").toElement(); //nMisses
                m_CSModel->setItem(0, 3, new QStandardItem(elem.text().mid(0, (elem.text().length()-1))));
            }
            else if(node.nodeName() == "strategyChoices"){
                QDomNodeList strategies = node.childNodes();
                m_StrategyModel->setRowCount(strategies.count());
                for(int i = 0; i < strategies.count(); ++ i){
                    QDomNode strategy = strategies.at(i);
                    if(strategy.isElement()){
                        QDomElement elem = strategy.namedItem("namespace").toElement();
                        m_StrategyModel->setItem(i, 0, new QStandardItem(elem.text()));

                        elem = strategy.namedItem("strategy").namedItem("name").toElement();
                        m_StrategyModel->setItem(i, 1, new QStandardItem(elem.text()));
                    }
                }
            }
            else{
                std::cerr << "parse route information: no match" << std::endl;
            }

            node = node.nextSibling(); //下一个兄弟节点
        }
    }
}
