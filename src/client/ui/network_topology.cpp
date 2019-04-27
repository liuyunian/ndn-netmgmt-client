#include "network_topology.h"
#include "ui_network_topology.h"
#include <src/client/ui/network_topology.moc>
#include <QMenu>
#include <memory>

#include "node.h"

#define R 80

NetworkTopology::NetworkTopology(std::vector<std::unique_ptr<Node>> & nodeList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkTopology),
    m_nodeList(nodeList){

    ui->setupUi(this);

    for(auto & node : m_nodeList){
        QLabel * label = new QLabel(this);
        label->setGeometry(node->getLeftPoint()->getX(), node->getLeftPoint()->getY()-30, 150, 20);
        label->setText(QString::fromStdString(node->getNodeName() + ": " + node->getNodePrefix()));
        m_labels.push_back(label);
    }
}

NetworkTopology::~NetworkTopology()
{
    delete ui;

    for(auto iter = m_labels.begin(); iter != m_labels.end(); ++ iter){
        delete (*iter);
    }
}

void NetworkTopology::paintEvent(QPaintEvent *event){
    m_painter = std::make_unique<QPainter>(this);
    m_painter->setPen(QPen(Qt::black,4,Qt::SolidLine));
    m_painter->setBrush(QBrush(Qt::gray,Qt::SolidPattern));

    for(auto & node : m_nodeList){
        m_painter->drawEllipse(node->getLeftPoint()->getX(), node->getLeftPoint()->getY(), R, R);
    }
}

void NetworkTopology::contextMenuEvent(QContextMenuEvent *event){
    int click_X = cursor().pos().x()-pos().x();
    int click_Y = cursor().pos().y()-pos().y();

    for(auto & node : m_nodeList){
        if((click_X >= node->getLeftPoint()->getX() && click_X <= node->getRightPoint()->getX()) 
            && (click_Y >= node->getLeftPoint()->getY() && click_Y <= node->getRightPoint()->getY())){
                QMenu * menu = new QMenu(this);
                
                QAction * routeInfor = new QAction("Route Information");
                QAction * CSInfor = new QAction("CS Information");
                QAction * packetInfor = new QAction("Packet Information");

                menu->addAction(routeInfor);
                menu->addAction(CSInfor);
                menu->addAction(packetInfor);

                connect(routeInfor, SIGNAL(triggered(bool)), node.get(), SLOT(onShowRouteInfor()));
                connect(CSInfor, SIGNAL(triggered(bool)), node.get(), SLOT(onShowCSInfor()));
                connect(packetInfor, SIGNAL(triggered(bool)), node.get(), SLOT(onShowPacketInfor()));

                menu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
                menu->show();

                break;
            }
    }
}
