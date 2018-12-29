#ifndef COLLECT_INFORMATION_H
#define COLLECT_INFORMATION_H

#include <QDialog>
#include <QLayout>

#include "ndn_client.hpp"

namespace Ui {
class CollectInformation;
}

class CollectInformation : public QDialog
{
    Q_OBJECT

public:
    explicit CollectInformation(std::shared_ptr<Client> & client, QWidget *parent = 0);
    ~CollectInformation();

private slots:
    void on_AddNodeInfor_clicked();

    void on_ButtonBox_accepted();

    void on_ButtonBox_rejected();

public:
    std::shared_ptr<Client> c_client;

private:
    Ui::CollectInformation *ui;
};

#endif // COLLECT_INFORMATION_H
