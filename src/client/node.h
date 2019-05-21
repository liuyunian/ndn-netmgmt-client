#ifndef NODE_H
#define NODE_H
#include <string>
#include <memory>
#include <QObject>
#include <QMenu>
#include <QPoint>

#include "traffic_infor.h"
#include "status_infor.h"

class Node : public QObject{
    Q_OBJECT //如果要用信号与槽机制必须要加这句话

public:
    Node(std::string & name, std::string & prefix, std::shared_ptr<QPoint> & center, std::shared_ptr<std::vector<std::string>> & neighbors);

    Node(const Node &) = delete;
    Node & operator=(const Node &) = delete;

    inline std::string getNodeName(){return m_name;}

    inline std::string getNodePrefix(){return m_prefix;}

    inline int getCenter_x(){return m_center->x();} 

    inline int getCenter_y(){return m_center->y();}

public slots:
    void onShowStatusInfor();

    void onShowTrafficInfor();

    void on_closeStatusWindow();

    void on_closeTrafficWindow();

    bool isAllClosed();

private:
    std::string m_name;
    std::string m_prefix;
    std::shared_ptr<QPoint> m_center;

    std::unique_ptr<Client> m_client;
    std::unique_ptr<StatusInfor> m_status;
    std::unique_ptr<TrafficInfor> m_traffic; //数据包信息窗口

public:
    std::shared_ptr<std::vector<std::string>> m_neighbors;
};

#endif // NODE_H
