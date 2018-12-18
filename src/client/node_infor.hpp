#ifndef NODE_INFOR_H
#define NODE_INFOR_H

#include <QMainWindow>
#include "ndn_client.hpp"

namespace Ui {
class NodeInfor;
}

class NodeInfor : public QMainWindow
{
    Q_OBJECT

public:
    explicit NodeInfor(QWidget *parent = 0);
    ~NodeInfor();

    /**
     * 设置指针m_client,指向client.cpp中创建的client实例
     * 本来是想在构造函数中初始化的，有问题！
    */
    void setPointToClient(std::shared_ptr<Client> client){u_client = client;}

private slots:
    void on_addNodeInfo_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::NodeInfor *ui;
    std::shared_ptr<Client> u_client;
};

#endif // NODE_INFOR_H
