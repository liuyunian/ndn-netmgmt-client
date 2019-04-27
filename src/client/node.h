#ifndef NODE_H
#define NODE_H
#include <string>
#include <memory>
#include <QObject>
#include <QMenu>

#include "route_information.h"
#include "cs_information.h"
#include "packet_information.h"

class Node : public QObject{
    Q_OBJECT //如果要用信号与槽机制必须要加这句话

private:
    struct Point{
        uint16_t m_x;
        uint16_t m_y;

        Point(uint16_t x, uint16_t y) : m_x(x), m_y(y){}

        inline u_int16_t getX(){return m_x;}

        inline u_int16_t getY(){return m_y;}
    };

public:
    Node(std::string & name, std::string & prefix, uint16_t x, uint16_t y);

    Node(const Node &) = delete;
    Node & operator=(const Node &) = delete;

    inline std::string getNodeName(){return m_name;}

    inline std::string getNodePrefix(){return m_prefix;}

    inline auto getLeftPoint(){
        return m_lPoint;
    } 

    inline auto getRightPoint(){
        return m_rPoint;
    }

public slots:
    void onShowRouteInfor();

    void onShowCSInfor();

    void onShowPacketInfor();

    void on_closeRouteWindow();

    void on_closeCSWindow();

    void on_closePacketWindow();

private:
    std::string m_name;
    std::string m_prefix;

    std::unique_ptr<RouteInformation> m_route; //路由信息窗口
    std::unique_ptr<CSInformation> m_cs; //缓存信息窗口
    std::unique_ptr<PacketInformation> m_packet; //数据包信息窗口

public:
    std::shared_ptr<Point> m_lPoint;
    std::shared_ptr<Point> m_rPoint;
};

#endif // NODE_H
