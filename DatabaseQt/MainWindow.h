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
    QMap<QString, DBController*> databases;
    QTabWidget* dbTabs;

    /// <summary>
    /// Wczytuje dane z bazy danych do zakładek w oknie głównym
    /// </summary>
    void setupTabs(const QString& tabName);
    /// <summary>
    /// Usuwa połączenie z bazą danych oraz czyści zakładki
    /// </summary>
    void deleteDatabaseConnection();
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private slots:
    /// <summary>
    ///  Otwiera okienko do wyboru pliku, a następnie łączy się z bazą danych
    /// </summary>
    void onLoadButtonClicked();
    void onCloseButtonClicked();
    void onDBCloseRequest(int index);
    void onTableRemoveRequest(int index);
};

#endif // MAINWINDOW_H