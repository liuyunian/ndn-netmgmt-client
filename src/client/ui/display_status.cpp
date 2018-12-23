#include "display_status.hpp"
#include "ui_display_status.h"

#include "node_status.hpp"
#include "ui_node_status.h"

#include <src/client/ui/display_status.moc>

#include "node_entry.hpp"

DisplayStatus::DisplayStatus(std::shared_ptr<Client> & client, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayStatus),
    d_client(client)
{
    ui->setupUi(this);
    for(std::shared_ptr<NodeEntry> &entry : d_client->m_nodeEntryList){
        NodeStatus * node = new NodeStatus();
        //TODO：填充数据
        node->setTitle(QString::fromStdString(entry->getNodeName()));
        this->ui->verticalLayout->addWidget(node);
    }
}

DisplayStatus::~DisplayStatus()
{
    delete ui;
}

