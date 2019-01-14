# ndn-netmgmt
对ndn网络节点进行管理

## server
#### 功能 
* 负责收集节点上的nfd的状态信息，之后打包成data包，发送给client
* 抓包功能，并将数据包信息打包成

## client 
#### 功能：
* 收集网络中节点的信息name、prefix
* 绘制管理网路的拓扑图
* 请求节点的nfd的状态信息（包括：FIB、RIB、CS），并在图形界面上显示节点的状态信息
* 请求节点的收发NDN数据包信息


