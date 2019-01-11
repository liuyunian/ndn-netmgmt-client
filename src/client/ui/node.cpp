#include "node.hpp"
#include "ui_node.h"
#include <src/client/ui/node.moc>

Node::Node(const std::string & name, const std::string & prefix, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::Node),
    n_nodeName(name),
    n_prefix(prefix)
{
    ui->setupUi(this);
    this -> setTitle(QString::fromStdString(name + ":" + prefix));
}

Node::~Node()
{
    delete ui;
}

void Node::on_requestRoute_clicked()
{
    this -> ui -> requestRoute -> setDisabled(true);

    n_route = new RouteInformation(n_prefix);

    connect(n_route, SIGNAL(closeWindow()),
            this, SLOT(on_closeRouteWindow()));
    n_route -> setWindowTitle(QString::fromStdString(n_nodeName + " Route Information"));

    n_route -> show();
}

void Node::on_requestCS_clicked()
{
    this -> ui -> requestCS -> setDisabled(true);

    n_cs = new CSInformation(n_prefix);
    connect(n_cs, SIGNAL(closeWindow()),
            this, SLOT(on_closeCSWindow()));
            
    n_cs -> setWindowTitle(QString::fromStdString(n_nodeName + " CS Information"));
    n_cs -> show();
}

void Node::on_requestCapture_clicked()
{

}

void Node::on_closeRouteWindow(){
    delete n_route;
    this -> ui -> requestRoute -> setEnabled(true);
}

void Node::on_closeCSWindow(){
    delete n_cs;
    this -> ui -> requestCS -> setEnabled(true);
}