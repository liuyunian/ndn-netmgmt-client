#ifndef PACKET_INFORMATION_HPP
#define PACKET_INFORMATION_HPP

#include <QDialog>
#include <QCloseEvent>

#include <iostream>
#include <thread>

#include "request_thread.hpp"

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
    void on_startCapture_clicked(); //点击开始捕获的槽函数

    void on_stopCapture_clicked(); //点击停止捕获的槽函数

    void on_startCaptureSuccessfully();

    void on_finishCapture();

    void on_displayCSInfor(QString);

private:
    Ui::PacketInformation *ui;

    RequestThread * p_request;
    std::string p_prefix;
};

#endif // PACKET_INFORMATION_HPP
