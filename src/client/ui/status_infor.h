#ifndef STATUS_INFOR_H
#define STATUS_INFOR_H

#include <QDialog>
#include <QStandardItemModel>
#include "ndn_client.h"

namespace Ui {
class StatusInfor;
}

class StatusInfor : public QDialog
{
    Q_OBJECT

public:
    explicit StatusInfor(std::string & name, std::string & prefix, QWidget *parent = 0);
    ~StatusInfor();

protected: 
    void closeEvent(QCloseEvent * event);

signals:
    void closeWindow();

private slots:
    void on_displayStatsInfor(QString);

private:
    Ui::StatusInfor *ui;
    QStandardItemModel * m_generalModel;
    QStandardItemModel * m_channelModel;
    QStandardItemModel * m_faceModel;
    QStandardItemModel * m_FIBModel;
    QStandardItemModel * m_RIBModel;
    QStandardItemModel * m_CSModel;
    QStandardItemModel * m_StrategyModel;

    std::string m_prefix;
    std::unique_ptr<Client> m_client;
};

#endif // STATUS_INFOR_H
