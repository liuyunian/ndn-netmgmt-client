#include "node.h"
#include <src/node.moc>
#include <iostream>

#include "ndn_client.h"

#define R 80

Node::Node(std::string & name, std::string & prefix, std::shared_ptr<QPoint> & center, std::shared_ptr<std::vector<std::string>> & neighbors) : 
    m_name(name),
    m_prefix(prefix),
    m_center(center),
    m_neighbors(neighbors),
    m_status(nullptr),
    m_traffic(nullptr),
    m_client(nullptr){
        // for(auto &item : (*m_neighbors)){
        //     std::cout << item << std::endl;
        // }
    }

void Node::onShowStatusInfor(){
    if(m_status == nullptr){
        m_status = std::make_unique<StatusInfor>(m_name, m_prefix);

        connect(m_status.get(), SIGNAL(closeWindow()),
                this, SLOT(on_closeStatusWindow()));

        m_status->setWindowTitle(QString::fromStdString(m_name + " Status Information"));
        m_status->show();
    }
}

void Node::onShowTrafficInfor(){
    if(m_traffic == nullptr){
        m_traffic = std::make_unique<TrafficInfor>(m_prefix);

        connect(m_traffic.get(), SIGNAL(closeWindow()),
                this, SLOT(on_closeTrafficWindow()));

        m_traffic -> setWindowTitle(QString::fromStdString(m_name + " Traffic Information"));
        m_traffic -> show();
    }
}

void Node::on_closeStatusWindow(){
    m_status = nullptr;
}

void Node::on_closeTrafficWindow(){
    m_traffic = nullptr;

    if(m_client == nullptr){
        m_client = std::make_unique<Client>(m_prefix);
    }
    m_client->stopRequestTrafficInfor();
}

bool Node::isAllClosed(){
    if(m_status == nullptr && m_traffic == nullptr){
        return true;
    }
    else{
        return false;
    }
}