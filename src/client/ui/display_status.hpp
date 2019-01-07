#ifndef DISPLAY_STATUS_H
#define DISPLAY_STATUS_H

#include <QWidget>

#include "ndn_management.hpp"
#include "request_thread.hpp"

namespace Ui {
class DisplayStatus;
}

class DisplayStatus : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayStatus(std::shared_ptr<Management> management, 
                            std::shared_ptr<RequestThread> request, 
                            QWidget *parent = 0);
    ~DisplayStatus();
signals:
    void requestImmediately();

private slots:
    void on_refresh_clicked();
    void onDisplayStatus(int i);

private:
    Ui::DisplayStatus *ui;
    std::shared_ptr<Management> d_management;
};

#endif // DISPLAY_STATUS_H
