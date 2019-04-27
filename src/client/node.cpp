#include "node.h"
#include <src/client/node.moc>
#include <iostream>

#define R 80

Node::Node(std::string & name, std::string & prefix, uint16_t x, uint16_t y) : 
    m_name(name),
    m_prefix(prefix),
    m_route(nullptr),
    m_cs(nullptr),
    m_packet(nullptr),
    m_lPoint(new Point(x, y)),
    m_rPoint(new Point(x+R, y+R)){}

void Node::onShowRouteInfor(){
    if(m_route == nullptr){
        m_route = std::make_unique<RouteInformation>(m_prefix);

        connect(m_route.get(), SIGNAL(closeWindow()),
                this, SLOT(on_closeRouteWindow()));

        m_route->setWindowTitle(QString::fromStdString(m_name + " Route Information"));
        m_route->show();
    }

}

void Node::onShowCSInfor(){
    if(m_cs == nullptr){
        m_cs = std::make_unique<CSInformation>(m_prefix);

        connect(m_cs.get(), SIGNAL(closeWindow()),
                this, SLOT(on_closeCSWindow()));
                
        m_cs -> setWindowTitle(QString::fromStdString(m_name + " CS Information"));
        m_cs -> show();
    }

}

void Node::onShowPacketInfor(){
    if(m_packet == nullptr){
        m_packet = std::make_unique<PacketInformation>(m_prefix);

        connect(m_packet.get(), SIGNAL(closeWindow()),
                this, SLOT(on_closePacketWindow()));

        m_packet -> setWindowTitle(QString::fromStdString(m_name + " Data Packet Information"));
        m_packet -> show();
    }

}

void Node::on_closeRouteWindow(){
    m_route = nullptr;
}

void Node::on_closeCSWindow(){
    m_cs = nullptr;
}

void Node::on_closePacketWindow(){
    m_packet = nullptr;
}