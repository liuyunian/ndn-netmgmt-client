#ifndef CS_INFORMATION_H
#define CS_INFORMATION_H

#include <QDialog>
#include <QStandardItemModel>
#include <QCloseEvent>
#include <memory>

#include "ndn_client.h"

namespace Ui {
class CSInformation;
}

class CSInformation : public QDialog
{
    Q_OBJECT

public:
    explicit CSInformation(std::string & prefix, QWidget *parent = 0);
    ~CSInformation();

signals:
    void closeWindow();

private slots:
    void on_displayCSInfor(QString);

protected: 
    void closeEvent(QCloseEvent * event);

private:
    Ui::CSInformation *ui;
    QStandardItemModel * m_CSModel;

    std::string m_prefix;
    std::unique_ptr<Client> m_client;
};

#endif // CS_INFORMATION_H
