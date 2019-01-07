#include <iostream>
#include <ndn-cxx/name.hpp>

#include "node_entry.hpp"

NodeEntry::NodeEntry(const std::string & name, const std::string & prefix)
: n_nodeName(name),
n_prefix((ndn::Name)prefix)
{}