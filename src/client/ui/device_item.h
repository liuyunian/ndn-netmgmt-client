#ifndef DEVICE_ITEM_H
#define DEVICE_ITEM_H

#include <QWidget>

namespace Ui {
class DeviceItem;
}

class DeviceItem : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceItem(QWidget *parent = 0);
    ~DeviceItem();

void appendPacketInfo(QChar origin, QString infor);

private:
    Ui::DeviceItem *ui;
};

#endif // DEVICE_ITEM_H
