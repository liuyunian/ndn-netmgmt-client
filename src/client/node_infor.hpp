#ifndef NODE_INFOR_H
#define NODE_INFOR_H

#include <QMainWindow>

namespace Ui {
class NodeInfor;
}

class NodeInfor : public QMainWindow
{
    Q_OBJECT

public:
    explicit NodeInfor(QWidget *parent = 0);
    ~NodeInfor();

private slots:
    void on_addNodeInfo_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::NodeInfor *ui;
    QString nodeName;
    QString nodePrefix;
};

#endif // NODE_INFOR_H
