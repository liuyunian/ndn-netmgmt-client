# ndn-netmgmt
开发一套对ndn节点信息管理的程序

## server
相当于producer,负责收集节点上的nfd的状态信息，之后打包成data包，发送给client

## client 
主要负责两个功能：
* 相当于consumer，想其他节点请求nfd的状态信息
* 将多个节点的状态信息汇总，并渲染显示
