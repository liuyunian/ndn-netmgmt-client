#ifndef NDN_CLIENT_H_
#define NDN_CLIENT_H_
#include <iostream>

#include "node.hpp"

/**
 * 这个类是对管理的节点和拓扑的抽象
*/
class Management
{
public:
    std::list<std::shared_ptr<Node>> m_nodeList; //Entry智能指针类型的列表
};
#endif