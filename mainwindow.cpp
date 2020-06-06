#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "csptestitem.h"

#include "cspgraphicviewanimationlistwidget.h"
#include "cspgraphicviewanimationindexitem.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    ui->listWidget->structModel()->setModel(CSpGraphicStructModel::ListModel(index));
}

void MainWindow::on_spinBox_editingFinished()
{
    updateItemSize();
}

void MainWindow::on_spinBox_2_editingFinished()
{
    updateItemSize();
}

void MainWindow::updateItemSize()
{
    ui->listWidget->structModel()->setIconItemSize(QSize(ui->spinBox->value(),
                                                         ui->spinBox_2->value()));
}

void MainWindow::init()
{
    ui->listWidget->resize(1000,500);
    ui->listWidget->show();

    ui->listWidget->structModel()->setModel(CSpGraphicStructModel::List_Model);
    ui->listWidget->structModel()->setSpaceX(6);
    ui->listWidget->structModel()->setSpaceY(6);

    for(int i = 0;i<1000;++i)
    {
        CSpTestItem* pItem = new CSpTestItem(nullptr);
        ui->listWidget->insertItem(pItem,-1,false);
    }
    ui->listWidget->refreshList();
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    updateItemSize();
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    updateItemSize();
}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    ui->listWidget->structModel()->setSpaceX(arg1);
}

void MainWindow::on_spinBox_4_valueChanged(int arg1)
{
    ui->listWidget->structModel()->setSpaceY(arg1);
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    ui->listWidget->blockAnimations(!checked);
}
