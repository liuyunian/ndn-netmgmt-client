#ifndef DISPLAY_STATUS_H
#define DISPLAY_STATUS_H

#include <QWidget>

#include "ndn_client.hpp"

namespace Ui {
class DisplayStatus;
}

class DisplayStatus : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayStatus(std::shared_ptr<Client> & client, QWidget *parent = 0);
    ~DisplayStatus();

private slots:
    void on_refresh_clicked();

private:
    Ui::DisplayStatus *ui;
    std::shared_ptr<Client> d_client;
};

#endif // DISPLAY_STATUS_H
