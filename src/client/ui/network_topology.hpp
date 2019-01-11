#ifndef NETWORK_TOPOLOGY_HPP
#define NETWORK_TOPOLOGY_HPP

#include <QDialog>
#include "ndn_management.hpp"

namespace Ui {
class NetworkTopology;
}

class NetworkTopology : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkTopology(std::shared_ptr<Management> management,
                                QWidget *parent = 0);
    ~NetworkTopology();

private:
    Ui::NetworkTopology *ui;
    std::shared_ptr<Management> t_management;
};

#endif // NETWORK_TOPOLOGY_HPP
