#ifndef TRAFFIC_INFOR_H
#define TRAFFIC_INFOR_H

#include <QDialog>
#include <map>

#include "ndn_client.h"
#include "device_item.h"

namespace Ui {
    class TrafficInfor;
}

class TrafficInfor : public QDialog
{
    Q_OBJECT

public:
    explicit TrafficInfor(std::string & prefix, QWidget *parent = 0);
    ~TrafficInfor();

protected: 
    void closeEvent(QCloseEvent * event);

signals:
    void closeWindow();

private slots:
    void on_displayDeviceInfor(QString);
    void on_displayPacketInfor(QString);

private:
    Ui::TrafficInfor *ui;

    std::string m_prefix;
    std::unique_ptr<Client> m_client;

    std::map<QString, DeviceItem *> m_deviceStore;
};

#endif // TRAFFIC_INFOR_H
