#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    canvas = qobject_cast<Canvas*>(ui->canvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_rectangleButton_clicked()
{
    canvas->setMode(DRAW_RECTANGLE);
}

void MainWindow::on_ellipseButton_clicked()
{
    canvas->setMode(DRAW_ELLIPSE);
}

void MainWindow::on_triangleButton_clicked()
{
    canvas->setMode(DRAW_TRIANGLE);
}

void MainWindow::on_connectionButton_clicked()
{
    canvas->setMode(CONNECT);
}

void MainWindow::on_moveButton_clicked()
{
    canvas->setMode(MOVE);
}

void MainWindow::on_deleteButton_clicked()
{
    canvas->setMode(ERASE);
}

void MainWindow::on_saveButton_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, "Сохранить рисунок", "", "JSON Files (*.json)");
    if (file.isEmpty()) return;

    try {
        canvas->save(file);
    } catch (...) {
        QMessageBox::critical(this, "Ошибка сохранения", "Файл не был сохранён!");
    }
}

void MainWindow::on_loadButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Открыть рисунок", "", "JSON Files (*.json)");
    if (file.isEmpty()) return;

    try {
        canvas->load(file);
    } catch (...) {
        QMessageBox::critical(this, "Ошибка загрузки", "Не получается открыть рисунок!");
    }
}
