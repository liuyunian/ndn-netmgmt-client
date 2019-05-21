#ifndef NETWORK_TOPOLOGY_H
#define NETWORK_TOPOLOGY_H

#include <QDialog>
#include <QPainter>
#include <QLabel>
#include <QPoint>
#include <memory>

#include "node.h"

namespace Ui {
class NetworkTopology;
}

class NetworkTopology : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkTopology(QWidget *parent = 0);
    NetworkTopology(const NetworkTopology &) = delete;
    NetworkTopology & operator=(const NetworkTopology &) = delete;

    ~NetworkTopology();

private:
    void readTopologyInfor();
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent * event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    Ui::NetworkTopology *ui;

    std::vector<std::unique_ptr<Node>> m_nodeList;
    std::map<std::string, std::shared_ptr<QPoint>> m_name_center_map;
};

#endif // NETWORK_TOPOLOGY_H
