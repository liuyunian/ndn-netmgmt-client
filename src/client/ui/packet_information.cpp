#include "packet_information.hpp"
#include "ui_packet_information.h"
#include <src/client/ui/packet_information.moc>

PacketInformation::PacketInformation(std::string & prefix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketInformation),
    p_prefix(prefix)
{
    ui->setupUi(this);

    ui -> displayStatus -> setDisabled(true);
    ui -> displayStatus -> setStyleSheet("QRadioButton::indicator {width: 20px; height: 20px; border-radius: 10px}"
                                        "QRadioButton::indicator:checked {background-color: #00FF00;}"
                                        "QRadioButton::indicator:unchecked {background-color: red;}");
}

PacketInformation::~PacketInformation()
{
    delete ui;
}

void PacketInformation::closeEvent(QCloseEvent * event)
{
    emit closeWindow();
}

// private slots
void PacketInformation::on_startCapture_clicked()
{
    /**
     * 创建请求抓取数据包信息的线程
    */
    p_request = new RequestThread(p_prefix);

    connect(p_request, SIGNAL(startCaptureSuccessfully()),
            this, SLOT(on_startCaptureSuccessfully()),
            Qt::QueuedConnection);

    connect(p_request, SIGNAL(finishCapture()),
            this, SLOT(on_finishCapture()),
            Qt::QueuedConnection);

    connect(p_request, SIGNAL(displayPacketInfor(QString)),
            this, SLOT(on_displayCSInfor(QString)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(stopCapture()),
            p_request, SLOT(on_stopCapture()),
            Qt::QueuedConnection);

    std::thread requestCaptureThread(&RequestThread::requestCaptureInformation, p_request);
    requestCaptureThread.detach();
}

void PacketInformation::on_stopCapture_clicked()
{
    emit stopCapture();
}

void PacketInformation::on_startCaptureSuccessfully(){
    ui -> displayStatus -> setChecked(true);
    ui -> startCapture -> setDisabled(true);
}

void PacketInformation::on_finishCapture(){
    delete p_request;
    ui -> displayStatus -> setChecked(false);
    ui -> startCapture -> setEnabled(true);
}

void PacketInformation::on_displayCSInfor(QString packetInfor){
    ui -> sendPacket -> append(packetInfor);
}

