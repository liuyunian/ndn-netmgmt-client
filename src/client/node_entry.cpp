#include <iostream>
#include <ndn-cxx/name.hpp>

#include "node_entry.hpp"

NodeEntry::NodeEntry(const std::string & name, const std::string & prefix)
: m_nodeName(name),
m_prefix((ndn::Name)prefix)
{
    m_nodeStatus = new NodeStatus();
}

NodeEntry::~NodeEntry(){
    delete m_nodeStatus; //new一定要和delete对应好
}