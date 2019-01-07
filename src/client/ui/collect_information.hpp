#ifndef COLLECT_INFORMATION_H
#define COLLECT_INFORMATION_H

#include <QDialog>
#include <QLayout>

#include "ndn_management.hpp"

namespace Ui {
class CollectInformation;
}

class CollectInformation : public QDialog
{
    Q_OBJECT

public:
    explicit CollectInformation(const std::shared_ptr<Management> management, QWidget *parent = 0);
    ~CollectInformation();

private slots:
    void on_AddNodeInfor_clicked();

    void on_ButtonBox_accepted();

    void on_ButtonBox_rejected();

private:
    Ui::CollectInformation *ui;
    std::shared_ptr<Management> c_management;
};

#endif // COLLECT_INFORMATION_H
