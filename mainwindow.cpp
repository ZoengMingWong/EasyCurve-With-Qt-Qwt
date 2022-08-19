#include "mainwindow.h"
#include "ui_mainwindow.h"

static _Tp initX[8] = { 0., 0.1, 0.3, 0.35, 0.6, 0.73, 0.8, 1.0 };

/* Demo view on
 * https://img-blog.csdnimg.cn/3575844e1fcc4d2aa7e8274c1b6d3141.gif
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_floatX(initX, initX + 8),
    m_floatY(8)
{
    ui->setupUi(this);
    m_plot = new PlotLayout(ui->tab, 0., 1.);

    for (int i = 0; i < 8; i++)
    {
        m_XEdits[i] = new QLineEdit(QString::number(initX[i], 'f', 4));
        m_YEdits[i] = new QLineEdit;
        m_XEdits[i]->setEnabled(false);
        ui->XLayout->addWidget(m_XEdits[i]);
        ui->YLayout->addWidget(m_YEdits[i]);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_setValBtn_clicked()
{
    m_plot->interp(m_floatX, m_floatY);
    for (int i = 0; i < 8; i++)
    {
        m_YEdits[i]->setText(QString::number(m_floatY[i], 'f', 4));
    }
}

void MainWindow::on_delBtn_clicked()
{
    m_plot->deleteSelectedPoint();
}

void MainWindow::on_resetBtn_clicked()
{
    m_plot->reset();
}
