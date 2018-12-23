#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/interest.hpp>
//qt
#include <QDomDocument>
#include <QFile>

#include "ndn_client.hpp"

Client::Client(ndn::Face& face)
: m_face(face)
{}

void Client::onData(const ndn::Data &data){
    std::cout << " recevice data " << std::endl;
    for(std::shared_ptr<NodeEntry> &entry : m_nodeEntryList){
        if(entry->getNodePrefix().equals(data.getName())){
            std::string dataContent((const char *)data.getContent().value()); //取出内容并转化成字符串形式
            /**
             * 截取子字符串
             * 目的是去除后面的乱码, 有乱码的话没办法进行xml解析
            */
            entry->setNodeStatus(dataContent.substr(0, dataContent.find("</nodeStatus>") + 13));
            std::cout << entry->getNodeName() << ":" <<std::endl;
            std::cout << entry->getNodeStatus() << std::endl;
            parseXML(entry->getNodeStatus());
        }
    }

}

void Client::parseXML(const std::string & strXML){
    QDomDocument doc;
    if(!doc.setContent(QString::fromStdString(strXML)))
    {
        std::cout << "read xml string error" << std::endl;
        return;
    }
    QDomNode node = doc.firstChild().firstChild(); //第二级子节点
    while(!node.isNull() && node.isElement())
    {
        if(node.nodeName() == "fib"){ //fib信息
            QDomNodeList fibEntrys = node.childNodes();
            for(int i = 0; i<fibEntrys.count(); i++)
            {
                QDomNode fibEntry = fibEntrys.at(i);
                if(fibEntry.isElement()){
                    QDomElement prefix = fibEntry.namedItem("prefix").toElement(); //prefix
                    std::cout << prefix.text().toStdString() << std::endl;

                    QDomNodeList nextHops = fibEntry.namedItem("nextHops").childNodes(); //nextHops,可能存在多条
                    for(int j = 0; j<nextHops.count(); j++){
                        QDomNode nextHop = nextHops.at(j);
                        if(nextHop.isElement()){
                            QDomElement faceId = nextHop.namedItem("faceId").toElement(); //faceId
                            std::cout << faceId.text().toStdString() << std::endl;
                            QDomElement cost = nextHop.namedItem("cost").toElement(); //cost
                            std::cout << cost.text().toStdString() << std::endl;
                        }
                    }
                }
            }
        }
        else if(node.nodeName() == "rib"){
            QDomNodeList ribEntrys = node.childNodes();
            for(int i = 0; i<ribEntrys.count(); i++)
            {
                QDomNode ribEntry = ribEntrys.at(i);
                if(ribEntry.isElement()){
                    QDomElement prefix = ribEntry.namedItem("prefix").toElement(); //prefix
                    std::cout << prefix.text().toStdString() << std::endl;

                    QDomNodeList routes = ribEntry.namedItem("routes").childNodes(); //routes,可能存在多条
                    for(int j = 0; j<routes.count(); j++){
                        QDomNode route = routes.at(j);
                        if(route.isElement()){
                            QDomElement faceId = route.namedItem("faceId").toElement(); //faceId
                            std::cout << faceId.text().toStdString() << std::endl;
                            QDomElement origin = route.namedItem("origin").toElement(); //origin
                            std::cout << origin.text().toStdString() << std::endl;
                            QDomElement cost = route.namedItem("cost").toElement();
                            std::cout << cost.text().toStdString() << std::endl;
                            QDomNode childInherit = route.namedItem("flags").firstChild();
                            std::cout << childInherit.nodeName().toStdString() << std::endl;
                        }
                    }
                }
            }
        }
        else if(node.nodeName() == "cs"){
            QDomElement capacity = node.namedItem("capacity").toElement(); //capacity
            std::cout << capacity.text().toStdString() << std::endl;
            QDomElement nEntries = node.namedItem("nEntries").toElement(); //nEntries
            std::cout << nEntries.text().toStdString() << std::endl;
            QDomElement nHits = node.namedItem("nHits").toElement(); //nHits
            std::cout << nHits.text().toStdString();
            QDomElement nMisses = node.namedItem("nMisses").toElement(); //nMisses
            std::cout << nMisses.text().toStdString() << std::endl;
        }
        else{
            std::cout << "error";
        }

        node=node.nextSibling(); //下一个兄弟节点
    }
}

void Client::onNack(){
    std::cout << " receive Nack " << std::endl;
}


void Client::onTimeOut(){
    std::cout << " time out " << std::endl;
}

void Client::createAndSendInterest(){
    for (const std::shared_ptr<NodeEntry> & entry : m_nodeEntryList){ //向每个节点都发送Interest
        ndn::Interest interest(entry->getNodePrefix()); //创建Interest对象，代表一个Interest包
        std::cout<<" interest's prefix is "<<interest.getName()<<std::endl;

        interest.setCanBePrefix(false);
        interest.setMustBeFresh(true); //设置兴趣包是否允许重传
        interest.setInterestLifetime(ndn::time::milliseconds(1000)); //设置兴趣包的存活时间
        interest.setNonce(std::rand()); //设置兴趣包中的随机数

        try {
            m_face.expressInterest(interest,
            std::bind(&Client::onData, this, _2), //收到data包之后的回调函数
            std::bind(&Client::onNack, this), //收到Nack包之后的回调函数
            std::bind(&Client::onTimeOut, this)); //超时之后的回调函数
        }
        catch (std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
    }    
}

void Client::start()
{
    createAndSendInterest();
    m_face.processEvents();
}