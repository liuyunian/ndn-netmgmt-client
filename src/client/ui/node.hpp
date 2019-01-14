#ifndef NODE_HPP
#define NODE_HPP

#include <QGroupBox>

#include "route_information.hpp"
#include "cs_information.hpp"
#include "packet_information.hpp"

namespace Ui {
class Node;
}

class Node : public QGroupBox
{
    Q_OBJECT

public:
    explicit Node(const std::string & name, const std::string & prefix, QWidget *parent = 0);

    ~Node();

    // @brief 获取节点名称
    std::string getNodeName(){return n_nodeName;}

    // @brief 获取节点监听的前缀
    std::string getNodePrefix(){return n_prefix;}

private slots:
    void on_requestRoute_clicked();

    void on_requestCS_clicked();

    void on_requestCapture_clicked();

    void on_closeRouteWindow();

    void on_closeCSWindow();

    void on_closePacketWindow();

private:
    Ui::Node *ui;
    std::string n_nodeName;
    std::string n_prefix;

    RouteInformation * n_route; //路由信息窗口
    CSInformation * n_cs; //缓存信息窗口
    PacketInformation * n_packet; //数据包信息窗口
};

#endif // NODE_HPP
