#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DBController.h"

#include <iostream>

#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QFileDialog>
#include <QGridLayout>
#include <QTextEdit>
#include <QLabel>
#include <qstandardpaths.h>
#include <QTableView>
#include <QSqlTableModel>
#include <QMessageBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    DBController* db;
    QTabWidget* tabs;

    void setupTabs();
    void deleteDatabaseConnection();
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private slots:
    void onLoadButtonClicked();
    void onCloseButtonClicked();
};

#endif // MAINWINDOW_H