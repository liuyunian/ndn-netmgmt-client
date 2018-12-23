#ifndef NODE_ENTRY_H_
#define NODE_ENTRY_H_

#include <ndn-cxx/name.hpp>

class NodeEntry{
public:
    NodeEntry(const std::string & name, const std::string & prefxi);

    /**
     * @brief 获取节点名称
    */
    std::string getNodeName(){return m_nodeName;}

    /**
     * @brief 获取节点监听的前缀
    */
    ndn::Name getNodePrefix(){return m_prefix;}

    /**
     * @brief 设置节点的状态信息
    */
    void setNodeStatus(const std::string & status){m_status = status;}

    /**
     * @brief 获取节点的状态信息
    */
    std::string getNodeStatus(){return m_status;}

private:
    std::string m_nodeName;
    ndn::Name m_prefix;
    std::string m_status;
};

#endif