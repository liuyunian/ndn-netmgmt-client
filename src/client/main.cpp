#include <QApplication>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <vector>
#include <memory>

#include "node.h"
#include "network_topology.h"

int main(int argc, char* argv[]){
    std::string line;
    std::ifstream fin("./src/client/topology/topology.txt");
    assert(fin.is_open());

    assert(std::getline(fin, line));
    std::stringstream ss(line);
    int size;
    ss >> size;

    std::vector<std::unique_ptr<Node>> m_nodeList(size);
    for(int i = 0; i < size; ++i){
        std::string name, prefix;
        int x, y;
        assert(std::getline(fin, line));
        std::stringstream ss(line);
        ss >> name >> prefix >> x >> y;
        m_nodeList[i] = std::make_unique<Node>(name, prefix, x, y);
    }

    QApplication app(argc, argv);

    NetworkTopology topology(m_nodeList);
    topology.show();

    return app.exec();
}
