#ifndef NODE_ENTRY_H_
#define NODE_ENTRY_H_

#include <ndn-cxx/name.hpp>

class NodeEntry{
public:
    NodeEntry(const std::string & name, const std::string & prefxi);

    /**
     * @brief 设置节点名称 
    */
    //void setNodeName(const std::string & name){m_nodeName = name;}

    /**
     * @brief 获取节点名称
    */
    std::string getNodeName(){return m_nodeName;}

    /**
     * @brief 设置节点监听的前缀 
    */
    //void setNodePrefix(const ndn::Name & prefix){m_prefix = prefix;}

    /**
     * @brief 获取节点监听的前缀
    */
    ndn::Name getNodePrefix(){return m_prefix;}

private:
    std::string m_nodeName;
    ndn::Name m_prefix;
};

#endif