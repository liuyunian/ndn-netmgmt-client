#ifndef ROUTE_INFORMATION_HPP
#define ROUTE_INFORMATION_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QCloseEvent>

#include <iostream>

#include "request_thread.hpp"

namespace Ui {
class RouteInformation;
}

class RouteInformation : public QDialog
{
    Q_OBJECT

public:
    explicit RouteInformation(std::string & prefix, QWidget *parent = 0);
    ~RouteInformation();

    /**
     * @brief 解析参数xml字符串，并在表格中填充数据
     * @param XML格式的路由信息
    */
    void parseRouteInfor(QString & routeInfor_xml);

protected: 
    void closeEvent(QCloseEvent * event);

signals:
    void closeWindow();

private slots:
    void on_displayRouteInfor(QString);

private:
    Ui::RouteInformation *ui;
    QStandardItemModel * r_FIBModel;
    QStandardItemModel * r_RIBModel;

    RequestThread * r_requestRoute;
    std::string r_prefix;
};

#endif // ROUTE_INFORMATION_HPP
