#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include "plotlayout.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_delBtn_clicked();
    void on_resetBtn_clicked();
    void on_setValBtn_clicked();

private:
    Ui::MainWindow *ui;

private:
    PlotLayout *m_plot = NULL;
    QLineEdit *m_XEdits[8];
    QLineEdit *m_YEdits[8];

    std::vector<_Tp> m_floatX;
    std::vector<_Tp> m_floatY;
};
#endif // MAINWINDOW_H
