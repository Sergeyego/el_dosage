#ifndef FORMRCP_H
#define FORMRCP_H

#include <QWidget>

namespace Ui {
class FormRcp;
}

class FormRcp : public QWidget
{
    Q_OBJECT

public:
    explicit FormRcp(QWidget *parent = nullptr);
    ~FormRcp();

private:
    Ui::FormRcp *ui;
};

#endif // FORMRCP_H
