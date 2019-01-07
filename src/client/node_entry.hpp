#ifndef NODE_ENTRY_H_
#define NODE_ENTRY_H_

#include <ndn-cxx/name.hpp>

#include "node_status.hpp"

class NodeEntry{
public:
    NodeEntry(const std::string & name, const std::string & prefxi);

    /**
     * @brief 获取节点名称
    */
    std::string getNodeName(){return n_nodeName;}

    /**
     * @brief 获取节点监听的前缀
    */
    ndn::Name getNodePrefix(){return n_prefix;}

    /**
     * @brief 设置节点的状态信息
    */
    void setNodeStatusInfor(const std::string & statusInfor){n_statusInfor = statusInfor;}

    /**
     * @brief 获取节点的状态信息
    */
    std::string getNodeStatusInfor(){return n_statusInfor;}

    /**
     * @brief 设置节点状态的qt显示对象指针
    */
    void setNodeStatusDisplay(std::shared_ptr<NodeStatus> statusDisplay){n_statusDisplay = statusDisplay;}

    /**
     * @brief 获取节点的状态qt显示对象指针
    */
    std::shared_ptr<NodeStatus> getNodeStatusDisplay(){return n_statusDisplay;}

private:
    std::string n_nodeName;
    ndn::Name n_prefix;
    std::string n_statusInfor;
    std::shared_ptr<NodeStatus> n_statusDisplay;
};

#endif