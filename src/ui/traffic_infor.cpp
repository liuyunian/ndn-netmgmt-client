#include "traffic_infor.h"
#include "ui_traffic_infor.h"
#include <src/ui/traffic_infor.moc>

#include <QDomDocument>

TrafficInfor::TrafficInfor(std::string & prefix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrafficInfor),
    m_prefix(prefix)
{
    ui->setupUi(this);

    m_client = std::make_unique<Client>(m_prefix);
    
    connect(m_client.get(), SIGNAL(displayDeviceInfor(QString)), 
            this, SLOT(on_displayDeviceInfor(QString)), 
            Qt::QueuedConnection);

    connect(m_client.get(), SIGNAL(displayPacketInfor(QString)), 
        this, SLOT(on_displayPacketInfor(QString)), 
        Qt::QueuedConnection);

    m_client->requestTrafficInfor();
}

TrafficInfor::~TrafficInfor()
{
    delete ui;
}

void TrafficInfor::closeEvent(QCloseEvent * event){
    emit closeWindow();
}

void TrafficInfor::on_displayDeviceInfor(QString deviceInfor){
    QDomDocument doc;
    if(!doc.setContent(deviceInfor))
    {
        std::cout << "read xml string error" << std::endl;
        return;
    }
    else{
        QDomNodeList devices = doc.firstChild().childNodes();
        for(int i = 0; i < devices.count(); ++ i){
            QDomElement elem = devices.at(i).toElement();
            auto deviceItem = new DeviceItem(this);
            QString tabName("Face");
            this->ui->tabWidget->addTab(deviceItem, tabName.append(QString::number(i)));
            m_deviceStore.insert({elem.text(), deviceItem});
        }
    }
}

void TrafficInfor::on_displayPacketInfor(QString packetInfor){
    QDomDocument doc;
    if(!doc.setContent(packetInfor))
    {
        std::cout << "read xml string error" << std::endl;
        return;
    }
    else{
        QDomNodeList packets = doc.firstChild().childNodes();
        for(int i = 0; i < packets.count(); ++ i){
            QDomNode packet = packets.at(i);
            if(packet.isElement()){
                QDomElement dev = packet.namedItem("head").namedItem("dev").toElement();
                QDomElement origin = packet.namedItem("head").namedItem("origin").toElement();
                QDomElement infor = packet.namedItem("body").toElement();
                auto iter = m_deviceStore.find(dev.text());
                if(iter != m_deviceStore.end()){
                    iter->second->appendPacketInfo(origin.text().at(0), infor.text());
                }
            }
        }
    }
}


