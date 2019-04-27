#ifndef PACKET_INFORMATION_H
#define PACKET_INFORMATION_H

#include <QDialog>
#include <QCloseEvent>

#include <iostream>
#include <memory>
#include "ndn_client.h"

namespace Ui {
class PacketInformation;
}

class PacketInformation : public QDialog
{
    Q_OBJECT

public:
    explicit PacketInformation(std::string & prefix, QWidget *parent = 0);
    ~PacketInformation();

protected: 
    void closeEvent(QCloseEvent * event);

signals:
    void closeWindow();

    void stopCapture();

private slots:
    void on_stopCapture_clicked(); //点击停止捕获的槽函数

    void on_displayPacketInfor(QString);

private:
    Ui::PacketInformation *ui;

    std::string m_prefix;
    std::unique_ptr<Client> m_client;
};

#endif // PACKET_INFORMATION_H
