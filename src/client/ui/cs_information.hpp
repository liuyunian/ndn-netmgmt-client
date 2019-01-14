#ifndef CS_INFORMATION_HPP
#define CS_INFORMATION_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QCloseEvent>

#include "request_thread.hpp"

namespace Ui {
class CSInformation;
}

class CSInformation : public QDialog
{
    Q_OBJECT

public:
    explicit CSInformation(std::string & prefix, QWidget *parent = 0);
    ~CSInformation();
    /**
     * @brief 解析参数xml字符串，并在表格中填充数据
     * @param XML格式的CS缓存信息
    */
    void parseCSInfor(QString & CSInfor_xml);

signals:
    void closeWindow();

private slots:
    void on_displayCSInfor(QString);

protected: 
    void closeEvent(QCloseEvent * event);

private:
    Ui::CSInformation *ui;
    QStandardItemModel * c_CSModel;

    RequestThread * c_request;
    std::string c_prefix;
};

#endif // CS_INFORMATION_HPP
