#include <QApplication>
#include "network_topology.h"

int main(int argc, char* argv[]){
    QApplication app(argc, argv);

    NetworkTopology topology;
    topology.show();

    return app.exec();
}
