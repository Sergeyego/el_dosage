#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include "tabmanager.h"
#include "rest/restconnection.h"
#include "rest_olap/cubewidget.h"
#include "formrcp/formrcp.h"
#include "widgets/formedtresttbl.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    TabManager *tabManager;
    void actAction(QAction *a, void (MainWindow::*sl)(), int lev);
    void loadSettings();
    void saveSettings();
    void loadAnalytics();
    void clearEmptyMenus();

private slots:
    void newAnalytics();
    void newFormRcp();
    void newFormEdtComp();

};
#endif // MAINWINDOW_H
