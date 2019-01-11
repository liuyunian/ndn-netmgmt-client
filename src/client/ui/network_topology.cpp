#include "network_topology.hpp"
#include "ui_network_topology.h"
#include <src/client/ui/network_topology.moc>

#include "node.hpp"
#include "ui_node.h"

NetworkTopology::NetworkTopology(std::shared_ptr<Management> management, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkTopology),
    t_management(management)
{
    ui->setupUi(this);
    for(std::shared_ptr<Node> node : t_management->m_nodeList){
        this -> ui -> verticalLayout -> addWidget(node.get());
    }
}

NetworkTopology::~NetworkTopology()
{
    delete ui;
}
