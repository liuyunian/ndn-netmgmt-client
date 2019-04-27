#include "packet_information.h"
#include "ui_packet_information.h"
#include <src/client/ui/packet_information.moc>

#include <QMessageBox>
#include <QCloseEvent>

PacketInformation::PacketInformation(std::string & prefix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInformation),
    m_prefix(prefix){
    ui->setupUi(this);

    ui->stopCapture->setDisabled(true);
    ui->displayStatus->setChecked(false);
    ui->displayStatus->setStyleSheet("QRadioButton::indicator {width: 20px; height: 20px; border-radius: 10px}"
                                        "QRadioButton::indicator:checked {background-color: #00FF00;}"
                                        "QRadioButton::indicator:unchecked {background-color: red;}");

    m_client = std::make_unique<Client>(m_prefix);
    
    connect(m_client.get(), SIGNAL(displayPacketInfor(QString)), 
            this, SLOT(on_displayPacketInfor(QString)), 
            Qt::QueuedConnection);

    connect(this, SIGNAL(stopCapture()),
            m_client.get(), SLOT(on_stopCapture()),
            Qt::QueuedConnection);

    m_client->requestCaptureInformation();

    ui->displayStatus->setChecked(true);
    ui->stopCapture->setEnabled(true);
}

PacketInformation::~PacketInformation(){
    delete ui;
}

void PacketInformation::closeEvent(QCloseEvent * event){
    if(ui->displayStatus->isChecked()){
        QMessageBox::warning(NULL, "warning", "please stop capture");
        event->ignore();
    }
    else{
        event->accept();
        emit closeWindow();
    }   
}

// private slots
void PacketInformation::on_stopCapture_clicked(){
    emit stopCapture();
    ui->displayStatus->setChecked(false);
    ui->stopCapture->setDisabled(true);
}

void PacketInformation::on_displayPacketInfor(QString pktInfor){
    QStringList pktInforList = pktInfor.split('\n');
    for(int i = 0; i < pktInforList.size()-1; ++i){
        if(pktInforList[i].at(0) == 'S'){
            ui->sendPacket->append(pktInforList[i]);
        }
        else{
            ui->reveivePacket->append(pktInforList[i]);
        }
    }
}
