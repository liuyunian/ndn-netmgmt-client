#include "network_topology.h"
#include "ui_network_topology.h"
#include <src/ui/network_topology.moc>

#include <QMenu>
#include <QPoint>
#include <QMessageBox>
#include <QCloseEvent>

#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>

#define R 40

NetworkTopology::NetworkTopology(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkTopology)
{
    readTopologyInfor();

    for(auto & node : m_nodeList){
        QLabel * label = new QLabel(this); //指定了parent,内存可以有parent释放
        label->setGeometry(node->getCenter_x()-R, node->getCenter_y()-(R+30), 150, 20);
        label->setText(QString::fromStdString(node->getNodeName() + ": " + node->getNodePrefix()));
    }

    ui->setupUi(this);
}

NetworkTopology::~NetworkTopology()
{
    delete ui;
}

void NetworkTopology::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setPen(QPen(Qt::black,3,Qt::SolidLine));
    painter.setBrush(QBrush(Qt::gray,Qt::SolidPattern));

    for(auto & node : m_nodeList){
        QPoint center(node->getCenter_x(), node->getCenter_y());

        for(auto & name : *(node->m_neighbors)){
            auto iter = m_name_center_map.find(name);
            if(iter != m_name_center_map.end()){
                painter.drawLine(center, *(iter->second));
            }
        }
    }

    painter.setPen(QPen(Qt::black, 4, Qt::SolidLine));

    for(auto & node : m_nodeList){
        QPoint center(node->getCenter_x(), node->getCenter_y());
        painter.drawEllipse(center, R, R);
    }
}

void NetworkTopology::closeEvent(QCloseEvent * event){
    bool status = true;
    for(auto & node : m_nodeList){
        if(!node->isAllClosed()){
            status = false;
            break;
        }
    }
    if(!status){
        QMessageBox::warning(NULL, "warning", "please close child window");
        event->ignore();
    }
    else{
        event->accept();
    }   
}

void NetworkTopology::contextMenuEvent(QContextMenuEvent *event){
    int click_X = cursor().pos().x()-pos().x();
    int click_Y = cursor().pos().y()-pos().y();

    for(auto & node : m_nodeList){
        int deff_x = click_X-node->getCenter_x();
        int deff_y = click_Y-node->getCenter_y();
        if(deff_x*deff_x+deff_y*deff_y <= R*R){
            QMenu * menu = new QMenu(this);
            
            QAction * statusInfor = new QAction("Status", this); //指定了parent,内存可以有parent释放
            QAction * trafficInfor = new QAction("Traffic", this);
            QAction * configMgmt = new QAction("Configuration", this);
            QAction * SecurityMgmt = new QAction("Security", this);
            QAction * rmNode = new QAction("Remove", this);

            menu->addAction(statusInfor);
            menu->addAction(trafficInfor);
            menu->addAction(configMgmt);
            menu->addAction(SecurityMgmt);
            menu->addAction(rmNode);

            connect(statusInfor, SIGNAL(triggered(bool)), node.get(), SLOT(onShowStatusInfor()));
            connect(trafficInfor, SIGNAL(triggered(bool)), node.get(), SLOT(onShowTrafficInfor()));

            menu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
            menu->show();

            break;
        }
    }
}

void NetworkTopology::readTopologyInfor(){
    std::string line;
    std::ifstream fin("./src/topology/topology.txt");
    assert(fin.is_open());

    assert(std::getline(fin, line));
    std::stringstream ss(line);
    int num;
    ss >> num;

    for(int i = 0; i < num; ++ i){
        std::string name, prefix;
        int x, y;
        assert(std::getline(fin, line));
        std::stringstream ss(line);
        ss >> name >> prefix >> x >> y;

        auto center = std::make_shared<QPoint>(x, y);
        m_name_center_map.insert({name, center});

        auto neighbors = std::make_shared<std::vector<std::string>>();
        std::string neighb;
        while(ss >> neighb){
            neighbors->push_back(neighb);
        }

        auto node = std::make_unique<Node>(name, prefix, center, neighbors);
        m_nodeList.push_back(std::move(node));
    }
}
