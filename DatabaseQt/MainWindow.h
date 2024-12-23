#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DBController.h"
#include "InsertRowsDialog.h"
#include "CreateNewDialog.h"
#include "CreateNewTableDialog.h"

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
#include <QMenu>
#include <QHeaderView>

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
    /// <summary>
    /// Pobiera nazwe aktualnie wybranej zakładki
    /// </summary>
    /// <returns>Nazwa aktywnej zakładki</returns>
    QString activeDatabaseName();
    /// <summary>
    /// Pobiera nazwe aktualnie wybranej zakładki tabeli w bazie danych
    /// </summary>
    /// <returns>Nazwa aktywnej zakładki</returns>
    QString activeTableName();
    void updateIcon();
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private slots:
    /// <summary>
    ///  Otwiera okienko do wyboru pliku, a następnie łączy się z bazą danych
    /// </summary>
    void onLoadButtonClicked();
    void onNewButtonClicked();
    void onDBCloseRequest(int index);
    void onTableRemoveRequest(int index);
    void onTableContextMenu(const QPoint& pos);
    void onTableCreatorRequest();
};

#endif // MAINWINDOW_H