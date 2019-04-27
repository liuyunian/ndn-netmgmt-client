#ifndef ROUTE_INFORMATION_H
#define ROUTE_INFORMATION_H

#include <QDialog>
#include <QStandardItemModel>
#include <QCloseEvent>

#include <iostream>
#include <memory>

#include "ndn_client.h"

namespace Ui {
class RouteInformation;
}

class RouteInformation : public QDialog
{
    Q_OBJECT

public:
    explicit RouteInformation(std::string & prefix, QWidget *parent = 0);
    ~RouteInformation();

protected: 
    void closeEvent(QCloseEvent * event);

signals:
    void closeWindow();

private slots:
    void on_displayRouteInfor(QString);

private:
    Ui::RouteInformation *ui;
    QStandardItemModel * m_FIBModel;
    QStandardItemModel * m_RIBModel;

    std::string m_prefix;
    std::unique_ptr<Client> m_client;
};

#endif // ROUTE_INFORMATION_H
