#include "device_item.h"
#include "ui_device_item.h"
#include <src/ui/device_item.moc>

#include <iostream>

DeviceItem::DeviceItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceItem)
{
    ui->setupUi(this);
}

DeviceItem::~DeviceItem()
{
    delete ui;
}

void DeviceItem::appendPacketInfo(QChar origin, QString infor){
    if(infor.isEmpty()){
        return;
    }

    if(origin == 'R'){
        ui->IncomingBrowser->append(infor);
    }
    else if(origin == "S"){
        ui->OutgoingBrowser->append(infor);
    }
    else{
        std::cerr << "origin is invalid" << std::endl;
    }
}
