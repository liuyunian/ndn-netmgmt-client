#ifndef NETWORK_TOPOLOGY_H
#define NETWORK_TOPOLOGY_H

#include <QDialog>
#include <QPainter>
#include <QLabel>
#include <memory>

#include "node.h"

namespace Ui {
class NetworkTopology;
}

class NetworkTopology : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkTopology(std::vector<std::unique_ptr<Node>> & nodeList, QWidget *parent = 0);
    NetworkTopology(const NetworkTopology &) = delete;
    NetworkTopology & operator=(const NetworkTopology &) = delete;

    ~NetworkTopology();

private:
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    Ui::NetworkTopology *ui;
    std::unique_ptr<QPainter> m_painter;
    std::vector<QLabel *> m_labels;

    std::vector<std::unique_ptr<Node>> & m_nodeList;
};

#endif // NETWORK_TOPOLOGY_H
